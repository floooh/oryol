#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::DepthStencilStateSetup
    @brief setup object for depthStencilState resources
*/
#include "Resource/Locator.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class DepthStencilStateSetup {
public:
    /// default constructor
    DepthStencilStateSetup();
    /// construct with locator
    DepthStencilStateSetup(const Resource::Locator& loc);
    
    /// set depth compare function
    void SetDepthCompareFunc(CompareFunc::Code f);
    /// get depth compare function
    CompareFunc::Code GetDepthCompareFunc() const;
    /// enable/disable depth write
    void SetDepthWriteEnabled(bool b);
    /// get depth write enabled/disabed
    bool GetDepthWriteEnabled() const;
    /// set stencil test enabled/disabled
    void SetStencilTestEnabled(bool b);
    /// get stencil test enabled/disabled
    bool GetStencilTestEnabled() const;
    
    /// set stencil fail stencil operation
    void SetStencilFailOp(Face::Code sides, StencilOp::Code op);
    /// get stencil failure stencil operation
    StencilOp::Code GetStencilFailOp(Face::Code side) const;
    /// set depth fail stencil operation
    void SetDepthFailOp(Face::Code sides, StencilOp::Code op);
    /// get depth fail stencil operation
    StencilOp::Code GetDepthFailOp(Face::Code side) const;
    /// set depth/stencil pass stencil operation
    void SetDepthStencilPassOp(Face::Code sides, StencilOp::Code op);
    /// get depth/stencil pass stencil operation
    StencilOp::Code GetDepthStencilPassOp(Face::Code side) const;
    /// set stencil compare function
    void SetStencilCompareFunc(Face::Code sides, CompareFunc::Code f);
    /// get stencil compare function
    CompareFunc::Code GetStencilCompareFunc(Face::Code side) const;
    /// set stencil read mask
    void SetStencilReadMask(Face::Code sides, uint32 mask);
    /// get stencil read mask
    uint32 GetStencilReadMask(Face::Code side) const;
    /// set stencil write mask
    void SetStencilWriteMask(Face::Code sides, uint32 mask);
    /// get stencil write mask
    uint32 GetStencilWriteMask(Face::Code side) const;
    /// set the stencil reference value
    void SetStencilRef(Face::Code sides, int32 ref);
    /// get the stencil reference value
    int32 GetStencilRef(Face::Code side) const;

    /// get the resource locator
    const Resource::Locator& GetLocator() const;
    
private:
    Resource::Locator loc;
    CompareFunc::Code depthCompareFunc;
    bool depthWriteEnabled;
    bool stencilTestEnabled;
    struct {
        StencilOp::Code stencilFailOp;
        StencilOp::Code depthFailOp;
        StencilOp::Code depthStencilPassOp;
        CompareFunc::Code stencilCompareFunc;
        uint32 stencilReadMask;
        uint32 stencilWriteMask;
        int32 stencilRef;
    } stencilState[Face::NumSides];
};

//------------------------------------------------------------------------------
inline const Resource::Locator&
DepthStencilStateSetup::GetLocator() const {
    return this->loc;
}

//------------------------------------------------------------------------------
inline CompareFunc::Code
DepthStencilStateSetup::GetDepthCompareFunc() const {
    return this->depthCompareFunc;
}

//------------------------------------------------------------------------------
inline bool
DepthStencilStateSetup::GetDepthWriteEnabled() const {
    return this->depthWriteEnabled;
}

//------------------------------------------------------------------------------
inline bool
DepthStencilStateSetup::GetStencilTestEnabled() const {
    return this->stencilTestEnabled;
}

//------------------------------------------------------------------------------
inline StencilOp::Code
DepthStencilStateSetup::GetStencilFailOp(Face::Code side) const {
    o_assert_range_dbg(side, Face::NumSides);
    return this->stencilState[side].stencilFailOp;
}

//------------------------------------------------------------------------------
inline StencilOp::Code
DepthStencilStateSetup::GetDepthFailOp(Face::Code side) const {
    o_assert_range_dbg(side, Face::NumSides);
    return this->stencilState[side].depthFailOp;
}

//------------------------------------------------------------------------------
inline StencilOp::Code
DepthStencilStateSetup::GetDepthStencilPassOp(Face::Code side) const {
    o_assert_range_dbg(side, Face::NumSides);
    return this->stencilState[side].depthStencilPassOp;
}

//------------------------------------------------------------------------------
inline CompareFunc::Code
DepthStencilStateSetup::GetStencilCompareFunc(Face::Code side) const {
    o_assert_range_dbg(side, Face::NumSides);
    return this->stencilState[side].stencilCompareFunc;
}

//------------------------------------------------------------------------------
inline uint32
DepthStencilStateSetup::GetStencilReadMask(Face::Code side) const {
    o_assert_range_dbg(side, Face::NumSides);
    return this->stencilState[side].stencilReadMask;
}

//------------------------------------------------------------------------------
inline uint32
DepthStencilStateSetup::GetStencilWriteMask(Face::Code side) const {
    o_assert_range_dbg(side, Face::NumSides);
    return this->stencilState[side].stencilWriteMask;
}

//------------------------------------------------------------------------------
inline int32
DepthStencilStateSetup::GetStencilRef(Face::Code side) const {
    o_assert_range_dbg(side, Face::NumSides);
    return this->stencilState[side].stencilRef;
}

} // namespace Render
} // namespace Oryol