//------------------------------------------------------------------------------
//  TextureBlockSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureBlockSetup.h"

namespace Oryol {

//------------------------------------------------------------------------------
TextureBlockSetup::TextureBlockSetup() :
Locator(Locator::NonShared()),
BindStage(ShaderStage::InvalidShaderStage),
BindSlot(InvalidIndex),
ShaderSelectionMask(0) {
    // empty
}

} // namespace Oryol
