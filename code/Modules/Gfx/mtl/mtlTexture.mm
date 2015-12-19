//------------------------------------------------------------------------------
//  mtlTexture.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlTexture.h"
#include "mtl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlTexture::mtlTexture() :
updateFrameIndex(-1),
numSlots(1),
activeSlot(0),
mtlSamplerState(nil),
mtlDepthTex(nil) {
    this->mtlTextures.Fill(nil);
}

//------------------------------------------------------------------------------
mtlTexture::~mtlTexture() {
    o_assert_dbg(nil == this->mtlSamplerState);
    o_assert_dbg(nil == this->mtlDepthTex);
    #if ORYOL_DEBUG
    for (int i = 0; i < NumSlots; i++) {
        o_assert_dbg(this->mtlTextures[i] == nil);
    }
    #endif
}

//------------------------------------------------------------------------------
void
mtlTexture::Clear() {
    textureBase::Clear();
    this->updateFrameIndex = -1;
    this->numSlots = 1;
    this->activeSlot = 0;
    this->mtlTextures.Fill(nil);
    this->mtlSamplerState = nil;
    this->mtlDepthTex = nil;
}

} // namespace _priv
} // namespace Oryol