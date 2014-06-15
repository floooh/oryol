//------------------------------------------------------------------------------
//  BlendStateSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "BlendStateSetup.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
BlendStateSetup::BlendStateSetup() :
blendingEnabled(false),
rgbSrcFactor(BlendFactor::One),
rgbDstFactor(BlendFactor::Zero),
rgbBlendOperation(BlendOperation::Add),
alphaSrcFactor(BlendFactor::One),
alphaDstFactor(BlendFactor::Zero),
alphaBlendOperation(BlendOperation::Add),
colorWriteMask(ColorWriteMask::All) {
    // empty
}

//------------------------------------------------------------------------------
BlendStateSetup::BlendStateSetup(const Resource::Locator& loc_) :
BlendStateSetup() {
    this->loc = loc_;
}

//------------------------------------------------------------------------------
void
BlendStateSetup::SetBlendingEnabled(bool b) {
    this->blendingEnabled = b;
}

//------------------------------------------------------------------------------
void
BlendStateSetup::SetSrcFactorRGB(BlendFactor::Code f) {
    this->rgbSrcFactor = f;
}

//------------------------------------------------------------------------------
void
BlendStateSetup::SetDstFactorRGB(BlendFactor::Code f) {
    this->rgbDstFactor = f;
}

//------------------------------------------------------------------------------
void
BlendStateSetup::SetOpRGB(BlendOperation::Code op) {
    this->rgbBlendOperation = op;
}

//------------------------------------------------------------------------------
void
BlendStateSetup::SetSrcFactorAlpha(BlendFactor::Code f) {
    this->alphaSrcFactor = f;
}

//------------------------------------------------------------------------------
void
BlendStateSetup::SetDstFactorAlpha(BlendFactor::Code f) {
    this->alphaDstFactor = f;
}

//------------------------------------------------------------------------------
void
BlendStateSetup::SetOpAlpha(BlendOperation::Code op) {
    this->alphaBlendOperation = op;
}

//------------------------------------------------------------------------------
void
BlendStateSetup::SetColorWriteMask(ColorWriteMask::Code mask) {
    this->colorWriteMask = mask;
}

} // namespace Render
} // namespace Oryol
