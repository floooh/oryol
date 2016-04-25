//------------------------------------------------------------------------------
//  d3d12ResAllocator.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12ResAllocator.h"
#include "d3d12Types.h"
#include "d3d12_impl.h"
#include "d3d12Config.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12ResAllocator::d3d12ResAllocator() {
    // empty
}

//------------------------------------------------------------------------------
d3d12ResAllocator::~d3d12ResAllocator() {
    o_assert(this->releaseQueue.Empty());
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::DestroyAll() {    
    freeItem item;
    while (!this->releaseQueue.Empty()) {
        this->releaseQueue.Dequeue(item);
        ULONG count = item.res->Release();
        o_assert_dbg(0 == count);
        o_dbg("> released d3d12 resource %p at shutdown\n", item.res);
    }
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::GarbageCollect(uint64_t frameIndex) {

    // release all resources from longer then NumFrames befores,
    // these are definitely no longer accessed by the GPU
    const uint64_t safeNumFrames = d3d12Config::NumFrames + 1;
    if (frameIndex > safeNumFrames) {
        const uint64_t minReleaseFrame = frameIndex - safeNumFrames;
        freeItem item;
        while (!this->releaseQueue.Empty() && (this->releaseQueue.Front().frameIndex < minReleaseFrame)) {
            this->releaseQueue.Dequeue(item);
            o_assert_dbg(item.res);
            item.res->Release();
            o_dbg("> released d3d12 resource %p at frame %d\n", item.res, frameIndex);
        }
    }
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResAllocator::createBuffer(ID3D12Device* d3d12Device, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, uint32_t size) {

    D3D12_HEAP_PROPERTIES heapProps;
    d3d12Types::initHeapProps(&heapProps, heapType);
    D3D12_RESOURCE_DESC desc;
    d3d12Types::initBufferResourceDesc(&desc, size);

    ID3D12Resource* d3d12Resource = nullptr;
    HRESULT hr = d3d12Device->CreateCommittedResource(
        &heapProps,                     // pHeapProperties
        D3D12_HEAP_FLAG_NONE,           // HeapFlags
        &desc,                          // pResourceDesc
        initialState,                   // InitialResourceState
        nullptr,                        // pOptimizedClearValue
        __uuidof(ID3D12Resource),
        (void**) &d3d12Resource);
    o_assert(SUCCEEDED(hr) && d3d12Resource);
    
    return d3d12Resource;
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::Transition(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* res, D3D12_RESOURCE_STATES fromState, D3D12_RESOURCE_STATES toState) {
    o_assert_dbg(cmdList);
    o_assert_dbg(res);
    o_assert_dbg(fromState != toState);

    D3D12_RESOURCE_BARRIER resBarrier;
    Memory::Clear(&resBarrier, sizeof(resBarrier));
    resBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    resBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    resBarrier.Transition.pResource = res;
    resBarrier.Transition.Subresource = 0;
    resBarrier.Transition.StateBefore = fromState;
    resBarrier.Transition.StateAfter = toState;
    cmdList->ResourceBarrier(1, &resBarrier);
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResAllocator::AllocUploadBuffer(ID3D12Device* d3d12Device, uint32_t size) {
    o_assert_dbg(d3d12Device);
    o_assert_dbg(size > 0);

    ID3D12Resource* buffer = this->createBuffer(d3d12Device, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, size);
    o_dbg("> created d3d12 upload buffer at %p\n", buffer);
    return buffer;
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResAllocator::AllocDefaultBuffer(ID3D12Device* d3d12Device, D3D12_RESOURCE_STATES state, uint32_t size) {
    o_assert_dbg(d3d12Device);
    o_assert_dbg(size > 0);

    ID3D12Resource* buffer = this->createBuffer(d3d12Device, D3D12_HEAP_TYPE_DEFAULT, state, size);
    o_dbg("> created d3d12 default buffer at %p\n", buffer);
    return buffer;
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResAllocator::AllocStaticBuffer(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* cmdList, uint64_t frameIndex, D3D12_RESOURCE_STATES state, const void* data, uint32_t size) {
    o_assert_dbg(d3d12Device);
    o_assert_dbg(cmdList);
    o_assert_dbg(size > 0);
    ID3D12Resource* buffer = this->AllocDefaultBuffer(d3d12Device, state, size);
    if (data) {
        this->CopyBufferData(d3d12Device, cmdList, frameIndex, state, buffer, nullptr, data, size);
    }
    return buffer;
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::CopyBufferData(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* cmdList, uint64_t frameIndex, D3D12_RESOURCE_STATES state, ID3D12Resource* dstResource, ID3D12Resource* optUploadBuffer, const void* data, uint32_t size) {
    o_assert_dbg(d3d12Device);
    o_assert_dbg(cmdList);
    o_assert_dbg(data && (size > 0));

    // NOTE: if an upload buffer is provided, but no dst buffer, then the data is only
    // copied into the upload buffer, but no copy-region command is aded to the cmdList.
    // This makes sense for data that is updated per frame, and the GPU renders directly
    // from the update buffer.
    o_assert_dbg(dstResource || optUploadBuffer);

    // transition dst buffer into copy-dest state
    if (dstResource) {
        this->Transition(cmdList, dstResource, state, D3D12_RESOURCE_STATE_COPY_DEST);
    }

    // create a temporary upload buffer if none is provided
    ID3D12Resource* uploadBuffer = optUploadBuffer;
    if (nullptr == uploadBuffer) {
        uploadBuffer = this->AllocUploadBuffer(d3d12Device, size);
        o_dbg("> created d3d12 upload buffer %p at frame %d\n", uploadBuffer, frameIndex);
    }

    // copy data into upload buffer
    void* dstPtr = nullptr;
    HRESULT hr = uploadBuffer->Map(0, nullptr, &dstPtr);
    o_assert(SUCCEEDED(hr) && dstPtr);
    Memory::Copy(data, dstPtr, size);
    uploadBuffer->Unmap(0, nullptr);

    // add upload command to command list and transition back to renderable state
    if (dstResource) {
        cmdList->CopyBufferRegion(dstResource, 0, uploadBuffer, 0, size);
        this->Transition(cmdList, dstResource, D3D12_RESOURCE_STATE_COPY_DEST, state);
    }

    // if an upload buffer was created, release it
    if (nullptr == optUploadBuffer) {
        this->ReleaseDeferred(frameIndex, uploadBuffer);
    }
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResAllocator::AllocRenderTarget(ID3D12Device* d3d12Device, int width, int height, PixelFormat::Code fmt, const ClearState& clearHint, int smpCount) {
    o_assert_dbg(d3d12Device);
    o_assert_dbg((width > 0) && (height > 0) && (smpCount > 0));
    o_assert_dbg(PixelFormat::IsValidRenderTargetColorFormat(fmt) || PixelFormat::IsValidTextureDepthFormat(fmt));

    D3D12_RESOURCE_DESC desc;
    d3d12Types::initRTResourceDesc(&desc, width, height, fmt, smpCount);
    D3D12_HEAP_PROPERTIES heapProps;
    d3d12Types::initHeapProps(&heapProps, D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_STATES initialState;
    if (PixelFormat::IsValidRenderTargetColorFormat(fmt)) {
        initialState = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    else {
        initialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    }

    D3D12_CLEAR_VALUE clearValue;
    const D3D12_CLEAR_VALUE* clearValuePtr = nullptr;
    if (clearHint.Actions != ClearState::None) {
        d3d12Types::initClearValue(&clearValue, fmt, clearHint);
        clearValuePtr = &clearValue;
    }

    ID3D12Resource* d3d12Resource = nullptr;
    HRESULT hr = d3d12Device->CreateCommittedResource(
        &heapProps,                         // pHeapProperties
        D3D12_HEAP_FLAG_NONE,               // HeapFlags
        &desc,                              // pResourceDesc
        initialState,                       // InitialResourceState
        clearValuePtr,                      // pOptimizedClearValue
        __uuidof(ID3D12Resource),
        (void**)&d3d12Resource);
    o_assert(SUCCEEDED(hr) && d3d12Resource);
    o_dbg("> created d3d12 render target resource at %p\n", d3d12Resource);

    return d3d12Resource;
}

//------------------------------------------------------------------------------
uint32_t
d3d12ResAllocator::ComputeTextureCopyFootprint(ID3D12Device* d3d12Device, const TextureSetup& setup) {
    o_assert_dbg(d3d12Device);
    o_assert_dbg((setup.Width > 0) && (setup.Height > 0) && (setup.NumMipMaps > 0));

    D3D12_RESOURCE_DESC desc;
    d3d12Types::initTextureResourceDesc(&desc, setup);

    const int numFaces = setup.Type == TextureType::TextureCube ? 6 : 1;
    const int numSubResources = numFaces * setup.NumMipMaps;

    UINT64 dstTotalSize = 0;
    d3d12Device->GetCopyableFootprints(
        &desc,              // pResourceDesc
        0,                  // FirstSubresource
        numSubResources,    // NumSubresources
        0,                  // BaseOffset
        nullptr,            // pLayouts
        nullptr,            // pNumRows
        nullptr,            // pRowSizeInBytes
        &dstTotalSize);     // pTotalBytes
    return (uint32_t) dstTotalSize;
}

//------------------------------------------------------------------------------
ID3D12Resource*
d3d12ResAllocator::AllocTexture(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* cmdList, uint64_t frameIndex, const TextureSetup& setup, const void* data, int size) {
    o_assert_dbg(d3d12Device);
    o_assert_dbg(cmdList);
    o_assert_dbg((setup.Width > 0) && (setup.Height > 0) && (setup.NumMipMaps > 0));
    o_assert_dbg(PixelFormat::IsValidTextureColorFormat(setup.ColorFormat));

    D3D12_RESOURCE_DESC desc;
    d3d12Types::initTextureResourceDesc(&desc, setup);
    D3D12_HEAP_PROPERTIES defaultHeapProps;
    d3d12Types::initHeapProps(&defaultHeapProps, D3D12_HEAP_TYPE_DEFAULT);
    
    // create the default-heap resource
    ID3D12Resource* d3d12Texture = nullptr;
    HRESULT hr = d3d12Device->CreateCommittedResource(
        &defaultHeapProps,              // pHeapProperties
        D3D12_HEAP_FLAG_NONE,           // HeapFlags
        &desc,                          // pResourceDesc
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,     // initialResourceState,
        nullptr,                        // pOptimizedClearValue
        __uuidof(ID3D12Resource),
        (void**)&d3d12Texture);
    o_assert(SUCCEEDED(hr) && d3d12Texture);

    // if upload data provided, create upload resource and copy data
    if (data) {
        this->CopyTextureData(d3d12Device, cmdList, frameIndex, d3d12Texture, nullptr, data, setup.ImageData, setup);
    }
    return d3d12Texture;
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::CopyTextureData(ID3D12Device* d3d12Device, 
    ID3D12GraphicsCommandList* cmdList, 
    uint64_t frameIndex, 
    ID3D12Resource* dstTexture, 
    ID3D12Resource* optUploadBuffer, 
    const void* data, 
    const ImageDataAttrs& offsetsAndSizes,
    const TextureSetup& setup) {

    o_assert_dbg(d3d12Device);
    o_assert_dbg(cmdList);
    o_assert_dbg(dstTexture);
    o_assert_dbg(data);
    o_assert_dbg(offsetsAndSizes.NumFaces > 0);
    o_assert_dbg(offsetsAndSizes.NumMipMaps > 0);
    HRESULT hr;

    // transition texture resource into copy-dest state
    this->Transition(cmdList, dstTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

    // request texture layout information
    const int numSubResources = offsetsAndSizes.NumFaces * offsetsAndSizes.NumMipMaps;
    const int maxNumSubResources = GfxConfig::MaxNumTextureFaces * GfxConfig::MaxNumTextureMipMaps;
    o_assert(numSubResources <= maxNumSubResources);
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT dstLayouts[maxNumSubResources] = { 0 };
    UINT dstNumRows[maxNumSubResources] = { 0 };
    UINT64 dstRowSizeInBytes[maxNumSubResources] = { 0 };
    UINT64 dstTotalSize = 0;
    D3D12_RESOURCE_DESC desc = dstTexture->GetDesc();
    d3d12Device->GetCopyableFootprints(
        &desc,              // pResourceDesc
        0,                  // FirstSubresource
        numSubResources,    // NumSubresources
        0,                  // BaseOffset
        dstLayouts,         // pLayouts
        dstNumRows,            // pNumRows
        dstRowSizeInBytes,     // pRowSizeInBytes
        &dstTotalSize);        // pTotalBytes

    // optionally create a temporary upload buffer
    ID3D12Resource* uploadBuffer = optUploadBuffer;
    if (nullptr == uploadBuffer) {
        uploadBuffer = this->AllocUploadBuffer(d3d12Device, uint32_t(dstTotalSize));
    }

    // copy the source data into the upload buffer
    D3D12_TEXTURE_COPY_LOCATION srcLoc, dstLoc;
    Memory::Clear(&srcLoc, sizeof(srcLoc));
    Memory::Clear(&dstLoc, sizeof(dstLoc));
    srcLoc.pResource = uploadBuffer;
    srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dstLoc.pResource = dstTexture;
    dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

    uint8_t* dstStartPtr = nullptr;
    hr = uploadBuffer->Map(0, nullptr, (void**)&dstStartPtr);
    o_assert(SUCCEEDED(hr) && dstStartPtr);

    const uint8_t* srcStartPtr = (const uint8_t*)data;
    int subResourceIndex = 0;
    for (int faceIndex = 0; faceIndex < offsetsAndSizes.NumFaces; faceIndex++) {
        for (int mipIndex = 0; mipIndex < offsetsAndSizes.NumMipMaps; mipIndex++, subResourceIndex++) {

            const uint8_t* srcPtr = srcStartPtr + offsetsAndSizes.Offsets[faceIndex][mipIndex];
            const uint32_t srcRowPitch = PixelFormat::RowPitch(setup.ColorFormat, setup.Width >> mipIndex);
            const uint32_t srcNumRows = offsetsAndSizes.Sizes[faceIndex][mipIndex] / srcRowPitch;
            o_assert_dbg(dstRowSizeInBytes[subResourceIndex] <= srcRowPitch);
            o_assert_dbg(dstRowSizeInBytes[subResourceIndex] <= dstLayouts[subResourceIndex].Footprint.RowPitch);
            o_assert_dbg((srcNumRows * srcRowPitch) == offsetsAndSizes.Sizes[faceIndex][mipIndex]);
            o_assert_dbg(srcNumRows == dstNumRows[subResourceIndex]);

            uint8_t* dstPtr = dstStartPtr + dstLayouts[subResourceIndex].Offset;
            const uint32_t dstRowSize = (const uint32_t)dstRowSizeInBytes[subResourceIndex];
            for (uint32_t rowIndex = 0; rowIndex < srcNumRows; rowIndex++) {
                o_assert_dbg((dstPtr + dstRowSize) <= (dstStartPtr + dstTotalSize));
                std::memcpy(dstPtr, srcPtr, dstRowSize);
                dstPtr += dstLayouts[subResourceIndex].Footprint.RowPitch;
                srcPtr += srcRowPitch;
            }

            // issue the upload command
            srcLoc.PlacedFootprint = dstLayouts[subResourceIndex];
            dstLoc.SubresourceIndex = subResourceIndex;
            cmdList->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, nullptr);
        }
    }
    uploadBuffer->Unmap(0, nullptr);

    // if temporary upload buffer was created, put it into the deferred release queue
    if (nullptr == optUploadBuffer) {
        this->ReleaseDeferred(frameIndex, uploadBuffer);
    }

    // transition texture resource into usable state
    this->Transition(cmdList, dstTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

//------------------------------------------------------------------------------
void
d3d12ResAllocator::ReleaseDeferred(uint64_t frameIndex, ID3D12Object* res) {
    o_assert_dbg(res);

    // place a new item in the free-queue, the actual release will happen
    // a few frames later inside the GarbageCollect() method
    // when the GPU is done with the resource
    this->releaseQueue.Enqueue(frameIndex, res);
    o_dbg("> free d3d12 resource %p at frame %d\n", res, frameIndex);
}

} // namespace _priv
} // namespace Oryol
