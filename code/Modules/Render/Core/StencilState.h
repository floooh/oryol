#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::StencilState
    @brief holds stencil-buffer render state for one face side
*/
#include "Core/Types.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class StencilState {
public:
    /// constructor
    StencilState();
    
    /// equality
    bool operator==(const StencilState& rhs) const;
    /// inequality
    bool operator!=(const StencilState& rhs) const;
    
    /// enable/disable stencil test
    void SetEnabled(bool b);
    /// get stencil test enabled flag
    bool GetEnabled() const;
    
    /// set stencil fail stencil operation
    void SetStencilFailOp(StencilOp::Code op);
    /// get stencil failure stencil operation
    StencilOp::Code GetStencilFailOp() const;
    /// set depth fail stencil operation
    void SetDepthFailOp(StencilOp::Code op);
    /// get depth fail stencil operation
    StencilOp::Code GetDepthFailOp() const;
    /// set depth/stencil pass stencil operation
    void SetDepthStencilPassOp(StencilOp::Code op);
    /// get depth/stencil pass stencil operation
    StencilOp::Code GetDepthStencilPassOp() const;
    /// set stencil compare function
    void SetCompareFunc(CompareFunc::Code f);
    /// get stencil compare function
    CompareFunc::Code GetCompareFunc() const;
    /// set stencil read mask
    void SetReadMask(uint8 mask);
    /// get stencil read mask
    uint8 GetReadMask() const;
    /// set stencil write mask
    void SetWriteMask(uint8 mask);
    /// get stencil write mask
    uint8 GetWriteMask() const;
    /// set the stencil reference value
    void SetRef(uint8 ref);
    /// get the stencil reference value
    uint8 GetRef() const;
    
    /// get 64-bit hash code
    uint64 GetHash() const;
    
private:
    union {
        struct {
            bool enabled : 1;
            unsigned int stencilFailOp : 3;
            unsigned int depthFailOp : 3;
            unsigned int depthStencilPassOp : 3;
            unsigned int compareFunc : 3;
            unsigned int readMask : 8;
            unsigned int writeMask : 8;
            unsigned int ref : 8;
        } bits;
        uint64 hash;
    };
};

//------------------------------------------------------------------------------
inline
StencilState::StencilState() {
    this->hash = 0;
    this->bits.enabled = false;
    this->bits.stencilFailOp = StencilOp::Keep;
    this->bits.depthFailOp = StencilOp::Keep;
    this->bits.depthStencilPassOp = StencilOp::Keep;
    this->bits.compareFunc = CompareFunc::Always;
    this->bits.readMask = 0xFF;
    this->bits.writeMask = 0xFF;
    this->bits.ref = 0;
}

//------------------------------------------------------------------------------
inline bool
StencilState::operator==(const StencilState& rhs) const {
    return this->hash == rhs.hash;
}

//------------------------------------------------------------------------------
inline bool
StencilState::operator!=(const StencilState& rhs) const {
    return this->hash != rhs.hash;
}

//------------------------------------------------------------------------------
inline void
StencilState::SetEnabled(bool b) {
    this->bits.enabled = b;
}

//------------------------------------------------------------------------------
inline bool
StencilState::GetEnabled() const {
    return this->bits.enabled;
}

//------------------------------------------------------------------------------
inline void
StencilState::SetStencilFailOp(StencilOp::Code op) {
    this->bits.stencilFailOp = op;
}

//------------------------------------------------------------------------------
inline StencilOp::Code
StencilState::GetStencilFailOp() const {
    return (StencilOp::Code) this->bits.stencilFailOp;
}

//------------------------------------------------------------------------------
inline void
StencilState::SetDepthFailOp(StencilOp::Code op) {
    this->bits.depthFailOp = op;
}

//------------------------------------------------------------------------------
inline StencilOp::Code
StencilState::GetDepthFailOp() const {
    return (StencilOp::Code) this->bits.depthFailOp;
}

//------------------------------------------------------------------------------
inline void
StencilState::SetDepthStencilPassOp(StencilOp::Code op) {
    this->bits.depthStencilPassOp = op;
}

//------------------------------------------------------------------------------
inline StencilOp::Code
StencilState::GetDepthStencilPassOp() const {
    return (StencilOp::Code) this->bits.depthStencilPassOp;
}

//------------------------------------------------------------------------------
inline void
StencilState::SetCompareFunc(CompareFunc::Code f) {
    this->bits.compareFunc = f;
}

//------------------------------------------------------------------------------
inline CompareFunc::Code
StencilState::GetCompareFunc() const {
    return (CompareFunc::Code) this->bits.compareFunc;
}

//------------------------------------------------------------------------------
inline void
StencilState::SetReadMask(uint8 mask) {
    this->bits.readMask = mask;
}

//------------------------------------------------------------------------------
inline uint8
StencilState::GetReadMask() const {
    return this->bits.readMask;
}

//------------------------------------------------------------------------------
inline void
StencilState::SetWriteMask(uint8 mask) {
    this->bits.writeMask = mask;
}

//------------------------------------------------------------------------------
inline uint8
StencilState::GetWriteMask() const {
    return this->bits.writeMask;
}

//------------------------------------------------------------------------------
inline void
StencilState::SetRef(uint8 ref) {
    this->bits.ref = ref;
}

//------------------------------------------------------------------------------
inline uint8
StencilState::GetRef() const {
    return this->bits.ref;
}

//------------------------------------------------------------------------------
inline uint64
StencilState::GetHash() const {
    return this->hash;
}

} // namespace Render
} // namespace Oryol
