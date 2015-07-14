#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTexture
    @ingroup _priv
    @brief Metal implementation of class texture
*/
#include "Gfx/Resource/textureBase.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class mtlTexture : public textureBase {
public:
    /// constructor
    mtlTexture();
    /// destructor
    ~mtlTexture();

    /// clear the object
    void Clear();

    ORYOL_OBJC_TYPED_ID(MTLTexture) mtlTex;
    ORYOL_OBJC_TYPED_ID(MTLSamplerState) mtlSamplerState;
};

} // namespace _priv
} // namespace Oryol
