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

//------------------------------------------------------------------------------
TextureBlockSetup
TextureBlockSetup::VS(int32 bindSlot, const Id& shd, uint32 selMask) {
    TextureBlockSetup setup;
    setup.BindStage = ShaderStage::VS;
    setup.BindSlot = bindSlot;
    setup.Shader = shd;
    setup.ShaderSelectionMask = selMask;
    return setup;
}

//------------------------------------------------------------------------------
TextureBlockSetup
TextureBlockSetup::FS(int32 bindSlot, const Id& shd, uint32 selMask) {
    TextureBlockSetup setup;
    setup.BindStage = ShaderStage::FS;
    setup.BindSlot = bindSlot;
    setup.Shader = shd;
    setup.ShaderSelectionMask = selMask;
    return setup;
}

} // namespace Oryol
