#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11TextureBlock
    @ingroup _priv
    @brief D3D11 implementation of textureBlock
*/
#include "Gfx/resource/textureBlockBase.h"
#include "Gfx/d3d11/d3d11_decl.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {

class d3d11TextureBlock : public textureBlockBase {
public:
    /// clear the object
    void Clear();

    ShaderStage::Code bindStage = ShaderStage::InvalidShaderStage;
    struct entry {
        int32 bindSlot = InvalidIndex;
        ID3D11ShaderResourceView* d3d11ShaderResourceView = nullptr;
        ID3D11SamplerState* d3d11SamplerState = nullptr;
    };
    int32 numEntries = 0;
    StaticArray<entry, GfxConfig::MaxNumTexturesPerStage> entries;
};

} // namespace _priv
} // namespace Oryol