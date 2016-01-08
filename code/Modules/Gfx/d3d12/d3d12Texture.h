#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Texture
    @ingroup _priv
    @brief D3D12 implementation of class texture
*/
#include "Resource/Id.h"
#include "Gfx/Resource/textureBase.h"
#include "Gfx/d3d12/d3d12_decl.h"
#include "Gfx/d3d12/d3d12Config.h"

namespace Oryol {
namespace _priv {

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
    static const int32 NumSlots = d3d12Config::NumFrames;
    uint64 updateFrameIndex;
    uint32 numSlots;
    uint32 activeSlot;
    StaticArray<slot, NumSlots> slots;

    ID3D12Resource* d3d12DepthBufferRes;
    D3D12_RESOURCE_STATES d3d12DepthBufferState;
    int rtvDescriptorSlot;
    int dsvDescriptorSlot;
};

} // namespace _priv
} // namespace Oryol
