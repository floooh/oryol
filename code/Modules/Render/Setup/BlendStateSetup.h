#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::BlendStateSetup
    @brief setup object for blendState resources
*/
#include "Resource/Locator.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class BlendStateSetup {
public:
    /// default constructor
    BlendStateSetup();
    /// construct with locator
    BlendStateSetup(const Resource::Locator& loc);
    
    /// enable/disable blending (default is false)
    void SetBlendingEnabled(bool b);
    /// get enabled/disable blending flag
    bool GetBlendingEnabled() const;
    
    /// set source RGB blend factor
    void SetSrcFactorRGB(BlendFactor::Code srcRGB);
    /// get source RGB blend factor
    BlendFactor::Code GetSrcFactorRGB() const;
    /// set destination RGB blend factor
    void SetDstFactorRGB(BlendFactor::Code dstRGB);
    /// get destination RGB blend factor
    BlendFactor::Code GetDstFactorRGB() const;
    /// set the RGB blend operation
    void SetOpRGB(BlendOperation::Code rgbOp);
    /// get the RGB blend operation
    BlendOperation::Code GetOpRGB() const;
    
    /// set source Alpha blend factor
    void SetSrcFactorAlpha(BlendFactor::Code srcAlpha);
    /// get source Alpha blend factor
    BlendFactor::Code GetSrcFactorAlpha() const;
    /// set destination Alpha blend factor
    void SetDstFactorAlpha(BlendFactor::Code dstAlpha);
    /// get destination Alpha blend factor
    BlendFactor::Code GetDstFactorAlpha() const;
    /// set Alpha blend operation
    void SetOpAlpha(BlendOperation::Code alphaOp);
    /// get Alpha blend operation
    BlendOperation::Code GetOpAlpha() const;
    
    /// set color write mask
    void SetColorWriteMask(ColorWriteMask::Code mask);
    /// get color write mask
    ColorWriteMask::Code GetColorWriteMask() const;
    
    /// get the resource locator
    const Resource::Locator& GetLocator() const;
    
private:
    Resource::Locator loc;
    bool blendingEnabled;

    BlendFactor::Code rgbSrcFactor;
    BlendFactor::Code rgbDstFactor;
    BlendOperation::Code rgbBlendOperation;
    
    BlendFactor::Code alphaSrcFactor;
    BlendFactor::Code alphaDstFactor;
    BlendOperation::Code alphaBlendOperation;

    ColorWriteMask::Code colorWriteMask;
};

//------------------------------------------------------------------------------
inline const Resource::Locator&
BlendStateSetup::GetLocator() const {
    return this->loc;
}
    
//------------------------------------------------------------------------------
inline bool
BlendStateSetup::GetBlendingEnabled() const {
    return this->blendingEnabled;
}

//------------------------------------------------------------------------------
inline BlendFactor::Code
BlendStateSetup::GetSrcFactorRGB() const {
    return this->rgbSrcFactor;
}

//------------------------------------------------------------------------------
inline BlendFactor::Code
BlendStateSetup::GetDstFactorRGB() const {
    return this->rgbDstFactor;
}

//------------------------------------------------------------------------------
inline BlendOperation::Code
BlendStateSetup::GetOpRGB() const {
    return this->rgbBlendOperation;
}

//------------------------------------------------------------------------------
inline BlendFactor::Code
BlendStateSetup::GetSrcFactorAlpha() const {
    return this->alphaSrcFactor;
}

//------------------------------------------------------------------------------
inline BlendFactor::Code
BlendStateSetup::GetDstFactorAlpha() const {
    return this->alphaDstFactor;
}

//------------------------------------------------------------------------------
inline BlendOperation::Code
BlendStateSetup::GetOpAlpha() const {
    return this->alphaBlendOperation;
}

//------------------------------------------------------------------------------
inline ColorWriteMask::Code
BlendStateSetup::GetColorWriteMask() const {
    return this->colorWriteMask;
}

} // namespace Render
} // namespace Oryol