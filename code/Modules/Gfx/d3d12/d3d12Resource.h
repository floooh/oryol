#pragma once
//------------------------------------------------------------------------------
#include "Gfx/Resource/resourceBase.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/d3d12/d3d12_decl.h"
#include "Gfx/d3d12/d3d12Config.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Mesh
    @ingroup _priv
    @brief D3D12 implementation of class mesh
*/
class d3d12Mesh : public meshBase {
public:
    /// destructor
    ~d3d12Mesh();

    /// clear the object (called from meshFactory::DestroyResource())
    void Clear();

    static const int NumSlots = d3d12Config::NumFrames;
    struct buffer {
        buffer() :
            updateFrameIndex(InvalidIndex),
            numSlots(1),
            activeSlot(0) {
            d3d12RenderBuffers.Fill(nullptr);
            d3d12UploadBuffers.Fill(nullptr);
        }
        uint64_t updateFrameIndex;
        uint8_t numSlots;
        uint8_t activeSlot;
        StaticArray<ID3D12Resource*, NumSlots> d3d12RenderBuffers;
        StaticArray<ID3D12Resource*, NumSlots> d3d12UploadBuffers;
    };
    static const int vb = 0;
    static const int ib = 1;
    StaticArray<buffer, 2> buffers;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Pipeline
    @ingroup _priv
    @brief D3D12 implementation of pipeline
*/
class d3d12Pipeline : public pipelineBase {
public:
    /// constructor
    d3d12Pipeline();
    /// destructor
    ~d3d12Pipeline();

    /// clear the object (called from pipelineFactory::DestroyResource())
    void Clear();

    ID3D12PipelineState* d3d12PipelineState;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d12PrimTopologyType;
    D3D12_PRIMITIVE_TOPOLOGY d3d12PrimTopology;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Shader
    @ingroup _priv
    @brief D3D12 implementation of shader
*/
class d3d12Shader : public shaderBase {
public:
    /// destructor
    ~d3d12Shader();
    /// clear the object
    void Clear();

    /// pointer to and size of compiled shader data
    struct shaderBlob {
        const void* ptr = nullptr;
        uint32_t size = 0;
    };
    shaderBlob vertexShader;
    shaderBlob pixelShader;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Texture
    @ingroup _priv
    @brief D3D12 implementation of class texture
*/
class d3d12Texture : public textureBase {
public:
    /// constructor
    d3d12Texture();
    /// destructor
    ~d3d12Texture();

    /// clear the object
    void Clear();

    struct slot {
        ID3D12Resource* d3d12TextureRes;
        ID3D12Resource* d3d12UploadBuffer;      // only if dynamic texture
        D3D12_RESOURCE_STATES d3d12TextureState;
    };
    static const int NumSlots = d3d12Config::NumFrames;
    uint64_t updateFrameIndex;
    uint32_t numSlots;
    uint32_t activeSlot;
    StaticArray<slot, NumSlots> slots;

    ID3D12Resource* d3d12DepthStencilTextureRes;
    D3D12_RESOURCE_STATES d3d12DepthStencilTextureState;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12RenderPass
    @ingroup _priv
    @brief D3D12 implementation of renderPass
*/
class d3d12RenderPass : public renderPassBase {
public:
    /// constructor
    d3d12RenderPass();
    /// destructor
    ~d3d12RenderPass();

    /// clear the object
    void Clear();

    /// render-target-view descriptor slots
    StaticArray<int, GfxConfig::MaxNumColorAttachments> rtvDescSlots;
    /// depth-stencil-view descriptor slot
    int dsvDescSlot;
};

} // namespace _priv
} // namespace Oryol
