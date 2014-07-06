#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::BlendState
    @brief describe alpha blending state
*/
#include "Core/Types.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class BlendState {
public:
    /// constructor
    BlendState();
    
    /// equality
    bool operator==(const BlendState& rhs) const;
    /// inequality
    bool operator!=(const BlendState& rhs) const;
    
    /// enable/disable blending (default is false)
    void SetEnabled(bool b);
    /// get enabled/disable blending flag
    bool GetEnabled() const;
    
    /// set source RGB blend factor
    void SetSrcFactorRGB(BlendFactor::Code srcFactorRGB);
    /// get source RGB blend factor
    BlendFactor::Code GetSrcFactorRGB() const;
    /// set destination RGB blend factor
    void SetDstFactorRGB(BlendFactor::Code dstFactorRGB);
    /// get destination RGB blend factor
    BlendFactor::Code GetDstFactorRGB() const;
    /// set the RGB blend operation
    void SetOpRGB(BlendOperation::Code opRGB);
    /// get the RGB blend operation
    BlendOperation::Code GetOpRGB() const;
    
    /// set source Alpha blend factor
    void SetSrcFactorAlpha(BlendFactor::Code srcFactorAlpha);
    /// get source Alpha blend factor
    BlendFactor::Code GetSrcFactorAlpha() const;
    /// set destination Alpha blend factor
    void SetDstFactorAlpha(BlendFactor::Code dstFactorAlpha);
    /// get destination Alpha blend factor
    BlendFactor::Code GetDstFactorAlpha() const;
    /// set Alpha blend operation
    void SetOpAlpha(BlendOperation::Code opAlpha);
    /// get Alpha blend operation
    BlendOperation::Code GetOpAlpha() const;
    
    /// set color write mask
    void SetColorWriteMask(ColorWriteMask::Code mask);
    /// get color write mask
    ColorWriteMask::Code GetColorWriteMask() const;
    
    /// get 32-bit hash code
    uint32 GetHash() const;
    
private:
    union {
        struct {
             bool enabled : 1;
             unsigned int srcFactorRGB : 4;
             unsigned int dstFactorRGB : 4;
             unsigned int opRGB : 2;
             unsigned int srcFactorAlpha : 4;
             unsigned int dstFactorAlpha : 4;
             unsigned int opAlpha : 2;
             unsigned int colorWriteMask : 4;
        } bits;
        uint32 hash;
    };
};

//------------------------------------------------------------------------------
inline
BlendState::BlendState() {
    this->hash = 0;
    this->bits.enabled = false;
    this->bits.srcFactorRGB = BlendFactor::One;
    this->bits.dstFactorRGB = BlendFactor::Zero;
    this->bits.opRGB = BlendOperation::Add;
    this->bits.srcFactorAlpha = BlendFactor::One;
    this->bits.dstFactorAlpha = BlendFactor::Zero;
    this->bits.opAlpha = BlendOperation::Add;
    this->bits.colorWriteMask = ColorWriteMask::All;
}

//------------------------------------------------------------------------------
inline bool
BlendState::operator==(const BlendState& rhs) const {
    return this->hash == rhs.hash;
}

//------------------------------------------------------------------------------
inline bool
BlendState::operator!=(const BlendState& rhs) const {
    return this->hash != rhs.hash;
}

//------------------------------------------------------------------------------
inline void
BlendState::SetEnabled(bool b) {
    this->bits.enabled = b;
}

//------------------------------------------------------------------------------
inline bool
BlendState::GetEnabled() const {
    return this->bits.enabled;
}

//------------------------------------------------------------------------------
inline void
BlendState::SetSrcFactorRGB(BlendFactor::Code srcFactorRGB) {
    this->bits.srcFactorRGB = srcFactorRGB;
}

//------------------------------------------------------------------------------
inline BlendFactor::Code
BlendState::GetSrcFactorRGB() const {
    return (BlendFactor::Code) this->bits.srcFactorRGB;
}

//------------------------------------------------------------------------------
inline void
BlendState::SetDstFactorRGB(BlendFactor::Code dstFactorRGB) {
    this->bits.dstFactorRGB = dstFactorRGB;
}

//------------------------------------------------------------------------------
inline BlendFactor::Code
BlendState::GetDstFactorRGB() const {
    return (BlendFactor::Code) this->bits.dstFactorRGB;
}

//------------------------------------------------------------------------------
inline void
BlendState::SetOpRGB(BlendOperation::Code opRGB) {
    this->bits.opRGB = opRGB;
}

//------------------------------------------------------------------------------
inline BlendOperation::Code
BlendState::GetOpRGB() const {
    return (BlendOperation::Code) this->bits.opRGB;
}

//------------------------------------------------------------------------------
inline void
BlendState::SetSrcFactorAlpha(BlendFactor::Code srcFactorAlpha) {
    this->bits.srcFactorAlpha = srcFactorAlpha;
}

//------------------------------------------------------------------------------
inline BlendFactor::Code
BlendState::GetSrcFactorAlpha() const {
    return (BlendFactor::Code) this->bits.srcFactorAlpha;
}

//------------------------------------------------------------------------------
inline void
BlendState::SetDstFactorAlpha(BlendFactor::Code dstFactorAlpha) {
    this->bits.dstFactorAlpha = dstFactorAlpha;
}

//------------------------------------------------------------------------------
inline BlendFactor::Code
BlendState::GetDstFactorAlpha() const {
    return (BlendFactor::Code) this->bits.dstFactorAlpha;
}

//------------------------------------------------------------------------------
inline void
BlendState::SetOpAlpha(BlendOperation::Code opAlpha) {
    this->bits.opAlpha = opAlpha;
}

//------------------------------------------------------------------------------
inline BlendOperation::Code
BlendState::GetOpAlpha() const {
    return (BlendOperation::Code) this->bits.opAlpha;
}

//------------------------------------------------------------------------------
inline void
BlendState::SetColorWriteMask(ColorWriteMask::Code mask) {
    this->bits.colorWriteMask = mask;
}

//------------------------------------------------------------------------------
inline ColorWriteMask::Code
BlendState::GetColorWriteMask() const {
    return (ColorWriteMask::Code) this->bits.colorWriteMask;
}

//------------------------------------------------------------------------------
inline uint32
BlendState::GetHash() const {
    return this->hash;
}

} // namespace Render
} // namespace Oryol