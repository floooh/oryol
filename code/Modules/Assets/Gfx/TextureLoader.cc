//------------------------------------------------------------------------------
//  TextureLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureLoader.h"

namespace Oryol {

//------------------------------------------------------------------------------
bool
TextureLoader::Accepts(const SetupAndStream<TextureSetup>& input) const {
    // FIXME
    return false;
}

//------------------------------------------------------------------------------
SetupAndStream<TextureSetup>
TextureLoader::Load(const SetupAndStream<TextureSetup>& input) const {
    // FIXME
    return input;
}

} // namespace Oryol