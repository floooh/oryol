#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::DepthStencilState
    @brief holds the complete depth and stencil render state
*/
#include "Render/Core/Enums.h"
#include "Render/Core/StencilState.h"

namespace Oryol {
namespace Render {
    
class DepthStencilState {
public:
    /// constructor
    DepthStencilState();

    /// equality
    bool operator==(const DepthStencilState& rhs) const;
    /// inequality
    bool operator!=(const DepthStencilState& rhs) const;
    
    /// set depth compare function
    void SetDepthCompareFunc(CompareFunc::Code f);
    /// get depth compare function
    CompareFunc::Code GetDepthCompareFunc() const;
    /// enable/disable depth write
    void SetDepthWriteEnabled(bool b);
    /// get depth write enabled/disabed
    bool GetDepthWriteEnabled() const;

    /// get the depth state hash
    uint8 GetDepthStateHash() const;
    
    /// read/write access to stencil state
    class StencilState& StencilState(Face::Code side);
    /// read-only access to stencil state
    const class StencilState& StencilState(Face::Code side) const;
    
private:
    class StencilState stencilState[Face::NumSides];
    union {
        struct {
            unsigned int depthCompareFunc : 3;
            bool depthWriteEnabled : 1;
        } bits;
        uint8 hash;
    };
};

//------------------------------------------------------------------------------
inline
DepthStencilState::DepthStencilState() {
    this->hash = 0;
    this->bits.depthCompareFunc = CompareFunc::Always;
    this->bits.depthWriteEnabled = false;
}

//------------------------------------------------------------------------------
inline bool
DepthStencilState::operator==(const DepthStencilState& rhs) const {
    return (this->hash == rhs.hash) &&
           (this->stencilState[Face::Front] == rhs.stencilState[Face::Front]) &&
           (this->stencilState[Face::Back] == rhs.stencilState[Face::Back]);
}

//------------------------------------------------------------------------------
inline bool
DepthStencilState::operator!=(const DepthStencilState& rhs) const {
    return (this->hash != rhs.hash) ||
           (this->stencilState[Face::Front] != rhs.stencilState[Face::Front]) ||
           (this->stencilState[Face::Back] != rhs.stencilState[Face::Back]);
}

//------------------------------------------------------------------------------
inline void
DepthStencilState::SetDepthCompareFunc(CompareFunc::Code f) {
    this->bits.depthCompareFunc = f;
}

//------------------------------------------------------------------------------
inline CompareFunc::Code
DepthStencilState::GetDepthCompareFunc() const {
    return (CompareFunc::Code) this->bits.depthCompareFunc;
}

//------------------------------------------------------------------------------
inline void
DepthStencilState::SetDepthWriteEnabled(bool b) {
    this->bits.depthWriteEnabled = b;
}

//------------------------------------------------------------------------------
inline bool
DepthStencilState::GetDepthWriteEnabled() const {
    return this->bits.depthWriteEnabled;
}

//------------------------------------------------------------------------------
inline uint8
DepthStencilState::GetDepthStateHash() const {
    return this->hash;
}

//------------------------------------------------------------------------------
inline class StencilState&
DepthStencilState::StencilState(Face::Code side) {
    o_assert_range_dbg(side, Face::NumSides);
    return this->stencilState[side];
}

//------------------------------------------------------------------------------
inline const class StencilState&
DepthStencilState::StencilState(Face::Code side) const {
    o_assert_range_dbg(side, Face::NumSides);
    return this->stencilState[side];
}

} // namespace Render
} // namespace Oryol
