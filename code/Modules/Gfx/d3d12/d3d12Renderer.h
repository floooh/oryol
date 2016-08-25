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
#include "Gfx/d3d12/d3d12ResAllocator.h"
#include "Gfx/d3d12/d3d12DescAllocator.h"
#include "Gfx/d3d12/d3d12SamplerCache.h"
#include "d3d12_decl.h"

namespace Oryol {
namespace _priv {

class texture;
class pipeline;
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

    /// test if a feature is supported
    bool queryFeature(GfxFeature::Code feat) const;
    /// commit current frame
    void commitFrame();
    /// get the current render target attributes
    const DisplayAttrs& renderTargetAttrs() const;

    /// apply a render target (default or offscreen)
    void applyRenderTarget(texture* rt, const ClearState& clearState);
    /// apply viewport
    void applyViewPort(int x, int y, int width, int height, bool originTopLeft);
    /// apply scissor rect
    void applyScissorRect(int x, int y, int width, int height, bool originTopLeft);
    /// apply draw state
    void applyDrawState(pipeline* pip, mesh** meshes, int numMeshes);
    /// apply a shader uniform block
    void applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, uint32_t layoutHash, const uint8_t* ptr, int byteSize);
    /// apply a textures
    void applyTextures(ShaderStage::Code bindStage, texture** textures, int numTextures);
    /// submit a draw call with primitive group index in current mesh
    void draw(int primGroupIndex);
    /// submit a draw call with direct primitive group
    void draw(const PrimitiveGroup& primGroup);
    /// submit a draw call for instanced rendering with primitive group index in current mesh
    void drawInstanced(int primGroupIndex, int numInstances);
    /// submit a draw call for instanced rendering with direct primitive group
    void drawInstanced(const PrimitiveGroup& primGroup, int numInstances);
    /// update vertex data
    void updateVertices(mesh* msh, const void* data, int numBytes);
    /// update index data
    void updateIndices(mesh* msh, const void* data, int numBytes);
    /// update texture data
    void updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSize);
    /// read pixels back from framebuffer, causes a PIPELINE STALL!!!
    void readPixels(void* buf, int bufNumBytes);

    /// wait for the previous frame to finish
    void frameSync();

    /// pointer to d3d12 device (owned by display mgr)
    ID3D12Device* d3d12Device;
    /// pointer to d3d12 command queue (owned by display mgr)
    ID3D12CommandQueue* d3d12CommandQueue;
    /// pointer to the d3d12 root signature (owned by render mgr)
    ID3D12RootSignature* d3d12RootSignature;
    /// allocator for D3D12 resources
    d3d12ResAllocator resAllocator;
    /// allocator for D3D12 descriptors
    d3d12DescAllocator descAllocator;
    /// cache for D3D12 samplers
    d3d12SamplerCache samplerCache;

    /// the current frame index, starts at 0 and is incremented in commitFrame
    uint64_t frameIndex;
    /// the current frame index module the max number of in-flight-frames
    uint32_t curFrameRotateIndex;
    /// get the current (frame-rotated) command list
    ID3D12GraphicsCommandList* curCommandList() const {
        return this->frameResources[this->curFrameRotateIndex].commandList;
    };
    /// get the current (frame-rotated) command allocator
    ID3D12CommandAllocator* curCommandAllocator() const {
        return this->frameResources[this->curFrameRotateIndex].commandAllocator;
    };
    /// the global render-target-view descriptor heap
    Id rtvHeap;
    /// the global depth-stencil-view descriptor heap
    Id dsvHeap;

    /// create the default render-targets
    void createDefaultRenderTargets(int width, int height);
    /// destroy the default render-targets
    void destroyDefaultRenderTargets();

    /// called by display mgr when a window resize happens, sets flag to resize framebuf at next possible time
    void resizeAtNextFrame(int newWidth, int newHeight);

private:
    /// create the frame synchronization objects
    void createFrameSyncObjects();
    /// destory frame sync objects
    void destroyFrameSyncObjects();
    /// create the root signature object
    void createRootSignature();
    /// destroy the root signature object
    void destroyRootSignature();
    /// create the per-frame resources
    void createFrameResources(int cbSize, int maxDrawCallsPerFrame);
    /// destroy the per-frame resource
    void destroyFrameResources();
    /// perform a render-target transition
    void rtTransition(ID3D12Resource* rt, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
    /// (re-)set the root signature and descriptor heaps on the current command list
    void resetRootSignatureAndDescriptorHeaps();
    /// perform actual framebuffer resize (called from frameSync())
    void doResize(int resizeWidth, int resizeHeight);

    bool valid;
    GfxSetup gfxSetup;
    gfxPointers pointers;
    bool rtValid;
    DisplayAttrs rtAttrs;

    texture* curRenderTarget;
    pipeline* curPipeline;
    mesh* curPrimaryMesh;

    // frame-sync objects
    ID3D12Fence* d3d12Fence;
    HANDLE fenceEvent;

    // default render target
    StaticArray<ID3D12Resource*, d3d12Config::NumFrames> backbufferSurfaces;
    ID3D12Resource* msaaSurface;
    ID3D12Resource* depthStencilSurface;
    StaticArray<Id, d3d12Config::NumFrames> renderTargetViews;
    StaticArray<int, d3d12Config::NumFrames> rtvDescriptorSlots;
    int dsvDescriptorSlot;
    int curBackBufferIndex;
    
    // root signature slots, ordered from low to high change-frequency
    enum rootParam {
        PSConstantBuffer0 = 0,
        PSConstantBuffer1,
        VSConstantBuffer0,
        VSConstantBuffer1,
        PSTextures,
        PSSamplers,
        VSTextures,
        VSSamplers,

        NumRootParams
    };

    // per-frame resources which are rotated
    struct frameResources {
        ID3D12CommandAllocator* commandAllocator = nullptr;
        ID3D12GraphicsCommandList* commandList = nullptr;
        ID3D12Resource* constantBuffer = nullptr;
        Id srvHeap;
        uint8_t* cbCpuPtr = nullptr;
        uint64_t cbGpuPtr = 0;
    };
    StaticArray<frameResources, d3d12Config::NumFrames> frameResources;
    int curCBOffset;
    int curSRVSlotIndex;

    bool resizeFlag;
    int resizeWidth;
    int resizeHeight;
};

} // namespace _priv
} // namespace Oryol
