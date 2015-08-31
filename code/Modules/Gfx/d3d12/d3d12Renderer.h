#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Renderer
    @ingroup _priv
    @brief D3D12 implementation of class renderer
*/
#include "Core/Types.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/Core/ClearState.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/d3d12/d3d12Config.h"
#include "Gfx/d3d12/d3d12ResourceAllocator.h"
#include "d3d12_decl.h"

namespace Oryol {
namespace _priv {

class texture;
class drawState;
class mesh;

class d3d12Renderer {
public:
    /// constructor
    d3d12Renderer();
    /// destructor
    ~d3d12Renderer();

    /// setup the renderer
    void setup(const GfxSetup& setup, const gfxPointers& ptrs);
    /// discard the renderer
    void discard();
    /// return true if renderer has been setup
    bool isValid() const;

    /// reset the internal state cache
    void resetStateCache();
    /// test if a feature is supported
    bool queryFeature(GfxFeature::Code feat) const;
    /// commit current frame
    void commitFrame();
    /// get the current render target attributes
    const DisplayAttrs& renderTargetAttrs() const;

    /// apply a render target (default or offscreen)
    void applyRenderTarget(texture* rt, const ClearState& clearState);
    /// apply viewport
    void applyViewPort(int32 x, int32 y, int32 width, int32 height, bool originTopLeft);
    /// apply scissor rect
    void applyScissorRect(int32 x, int32 y, int32 width, int32 height, bool originTopLeft);
    /// apply draw state
    void applyDrawState(drawState* ds);
    /// apply a shader uniform block
    void applyUniformBlock(int32 blockIndex, int64 layoutHash, const uint8* ptr, int32 byteSize);
    /// submit a draw call with primitive group index in current mesh
    void draw(int32 primGroupIndex);
    /// submit a draw call with direct primitive group
    void draw(const PrimitiveGroup& primGroup);
    /// submit a draw call for instanced rendering with primitive group index in current mesh
    void drawInstanced(int32 primGroupIndex, int32 numInstances);
    /// submit a draw call for instanced rendering with direct primitive group
    void drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances);
    /// update vertex data
    void updateVertices(mesh* msh, const void* data, int32 numBytes);
    /// update index data
    void updateIndices(mesh* msh, const void* data, int32 numBytes);
    /// read pixels back from framebuffer, causes a PIPELINE STALL!!!
    void readPixels(void* buf, int32 bufNumBytes);

    /// invalidate currently bound mesh state
    void invalidateMeshState();
    /// invalidate currently bound shader program state
    void invalidateShaderState();
    /// invalidate currently bound draw state 
    void invalidateDrawState();
    /// invalidate currently bound texture state
    void invalidateTextureState();

    /// wait for the previous frame to finish
    void frameSync();

    /// pointer to d3d12 device (owned by display mgr)
    ID3D12Device* d3d12Device;
    /// pointer to d3d12 command queue (owned by display mgr)
    ID3D12CommandQueue* d3d12CommandQueue;
    /// pointer to d3d12 command allocator (owned by render mgr)
    ID3D12CommandAllocator* d3d12CommandAllocator;
    /// pointer to d3d12 command list (owned by render mgr)
    ID3D12GraphicsCommandList* d3d12CommandList;
    /// pointer to the d3d12 root signature (owned by render mgr)
    ID3D12RootSignature* d3d12RootSignature;
    // resource allocator for D3D12 objects
    d3d12ResourceAllocator d3d12Allocator;

    /// the current frame index, starts at 0 and is incremented in commitFrame
    uint64 frameIndex;

private:
    /// create the d3d12 command allocator object
    void createCommandAllocator();
    /// destroy d3d12 command allocator object
    void destroyCommandAllocator();
    /// create the d3d12 command list object
    void createCommandList();
    /// destroy the d3d12 command allocator object
    void destroyCommandList();
    /// create the frame synchronization objects
    void createFrameSyncObjects();
    /// destory frame sync objects
    void destroyFrameSyncObjects();
    /// create the default render-targets
    void createDefaultRenderTargets();
    /// destroy the default render-targets
    void destroyDefaultRenderTargets();
    /// create the root signature object
    void createRootSignature();
    /// destroy the root signature object
    void destroyRootSignature();
    /// create the per-frame resources
    void createFrameResources(int32 cbSize, int32 maxDrawCallsPerFrame);
    /// destroy the per-frame resource
    void destroyFrameResources();
    /// create the sampler-descriptor-heap
    void createSamplerDescriptorHeap();
    /// destroy the sampler-descriptor-heap
    void destroySamplerDescriptorHeap();

    bool valid;
    gfxPointers pointers;
    bool rtValid;
    DisplayAttrs rtAttrs;

    texture* curRenderTarget;
    drawState* curDrawState;

    // frame-sync objects
    ID3D12Fence* d3d12Fence;
    HANDLE fenceEvent;

    // default render target
    StaticArray<ID3D12Resource*, d3d12Config::NumFrames> d3d12RenderTargets;
    ID3D12DescriptorHeap* d3d12RTVHeap;
    int32 rtvDescriptorSize;
    uint32 curBackbufferIndex;
    
    // samplers are not per-frame!
    ID3D12DescriptorHeap* samplerDescHeap;

    // per-frame resources which are rotated
    struct frameResources {
        ID3D12Resource* constantBuffer = nullptr;
        uint8* constantBufferPtr = nullptr;
        ID3D12DescriptorHeap* cbvSrvHeap = nullptr;
    };
    StaticArray<frameResources, d3d12Config::NumFrames> d3d12FrameResources;
};

} // namespace _priv
} // namespace Oryol
