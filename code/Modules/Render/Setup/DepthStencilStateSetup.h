#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::DepthStencilStateSetup
    @brief setup object for DepthStencilState resources
*/
#include "Resource/Locator.h"

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
    DepthFunc::Code GetDepthCompareFunc() const;
    /// enable/disable depth write
    void SetDepthWriteEnabled(bool b);
    /// get depth write enabled/disabed
    bool GetDepthWriteEnabled() const;
    
    /// set stencil fail stencil operation
    void SetStencilFailOp(Face::Code sides, StencilOperation::Code op);
    /// get stencil failure stencil operation
    StencilOperation::Code GetStencilFailOp(Face::Code side) const;
    /// set depth fail stencil operation
    void SetDepthFailOp(Face::Code sides, StencilOperation::Code op);
    /// get depth fail stencil operation
    StencilOperation::Code GetDepthFailOp(Face::Code side) const;
    /// set depth/stencil pass stencil operation
    void SetDepthStencilPassOp(Face:Code sides, StencilOperation::Code op);
    /// get depth/stencil pass stencil operation
    StencilOperation::Code GetDepthStencilPassOp(Face::Code side) const;
    /// set stencil compare function
    void SetStencilCompareFunc(Face::Code side, CompareFunc::Code f);
    /// get stencil compare function
    CompareFunc::Code GetStencilCompareFunc() const;
    /// set stencil read mask
    void SetStencilReadMask(Face::Code sides, uint32 mask);
    /// get stencil read mask
    uint32 GetStencilReadMask(Face::Code side) const;
    /// set stencil write mask
    void SetStencilWriteMask(Face::Code sides, uint32 mask);
    /// get stencil write mask
    uint32 GetStencilWriteMask(Face::Code side) const;
    
private:
    Resource::Locator loc;
    CompareFunc::Code depthCompareFunc;
    bool depthWriteEnabled;
    struct {
        StencilOperation::Code stencilFailOp;
        StencilOperation::Code depthFailOp;
        StencilOperation::Code depthStencilPassOp;
        CompareFunc::Code stencilCompareFunc;
        uint32 stencilReadMask;
        uint32 stencilWriteMask;
    } stencilState[Face::NumSides];
};
    
} // namespace Render
} // namespace Oryol