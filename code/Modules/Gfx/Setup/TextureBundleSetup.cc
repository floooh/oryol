//------------------------------------------------------------------------------
//  TextureBundleSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureBundleSetup.h"

namespace Oryol {

//------------------------------------------------------------------------------
TextureBundleSetup::TextureBundleSetup() :
Locator(Locator::NonShared()),
ShaderSelectionMask(0) {
    // empty
}

//------------------------------------------------------------------------------
TextureBundleSetup
TextureBundleSetup::FromShader(const Id& shd, uint32 selMask) {
    TextureBundleSetup setup;
    setup.Shader = shd;
    setup.ShaderSelectionMask = selMask;
    return setup;
}

} // namespace Oryol
