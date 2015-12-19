#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTexture
    @ingroup _priv
    @brief Metal implementation of class texture
*/
#include "Gfx/Resource/textureBase.h"
#include "Core/Containers/StaticArray.h"
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

    static const int32 NumSlots = GfxConfig::MtlMaxInflightFrames;
    int32 updateFrameIndex;
    uint8 numSlots;
    uint8 activeSlot;
    StaticArray<ORYOL_OBJC_TYPED_ID(MTLTexture), NumSlots> mtlTextures;

    ORYOL_OBJC_TYPED_ID(MTLSamplerState) mtlSamplerState;
    ORYOL_OBJC_TYPED_ID(MTLTexture) mtlDepthTex;
};

} // namespace _priv
} // namespace Oryol
