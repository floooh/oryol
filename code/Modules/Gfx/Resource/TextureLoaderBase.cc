//------------------------------------------------------------------------------
//  TextureLoaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureLoaderBase.h"

namespace Oryol {

OryolClassImpl(TextureLoaderBase);

//------------------------------------------------------------------------------
bool
TextureLoaderBase::Supports(GfxResourceType::Code resType) const {
    return GfxResourceType::Texture == resType;
}

//------------------------------------------------------------------------------
bool
TextureLoaderBase::Accepts(const SetupAndStream<TextureSetup>& input) const {
    return false;
}

//------------------------------------------------------------------------------
SetupAndStream<TextureSetup>
TextureLoaderBase::Load(const SetupAndStream<TextureSetup>& input) const {
    return input;
}

} // namespace Oryol