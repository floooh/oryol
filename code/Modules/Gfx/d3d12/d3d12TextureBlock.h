#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12TextureBlock
    @ingroup _priv
    @brief D3D12 implementation of textureBlock
*/
#include "Gfx/resource/textureBlockBase.h"
#include "Resource/Id.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
namespace _priv {

class d3d12TextureBlock : public textureBlockBase {
public:
    /// clear the object
    void Clear();

    ShaderStage::Code bindStage = ShaderStage::InvalidShaderStage;
    class Id d3d12SRVDescriptor;
};

} // namespace _priv
} // namespace Oryol