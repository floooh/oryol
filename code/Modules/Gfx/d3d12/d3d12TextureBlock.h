#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12TextureBlock
    @ingroup _priv
    @brief D3D12 implementation of textureBlock
*/
#include "Gfx/resource/textureBlockBase.h"

namespace Oryol {
namespace _priv {

class d3d12TextureBlock : public textureBlockBase {
public:
    /// clear the object
    void Clear();

    // FIXME!
};

} // namespace _priv
} // namespace Oryol