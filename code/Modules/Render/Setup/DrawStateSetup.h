#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::DrawStateSetup
    @brief setup object for DrawState resources
*/
#include "Resource/Locator.h"
#include "Resource/Id.h"

namespace Oryol {
namespace Render {

class DrawStateSetup {
public:
    /// default constructor
    DrawStateSetup();
    /// construct with locator
    DrawStateSetup(const Resource::Locator& loc);
    /// construct with locator and resources
    DrawStateSetup(const Resource::Locator& loc,
                   const Resource::Id& depthStencilState,
                   const Resource::Id& blendState,
                   const Resource::Id& mesh,
                   const Resource::Id& prog, uint32 progSelMask);
    
    /// set the depth-stencil state
    void SetDepthStencilState(const Resource::Id& depthStencilState);
    /// get the depth-stencil state
    const Resource::Id& GetDepthStencilState() const;
    /// set the blend state
    void SetBlendState(const Resource::Id& blendState);
    /// get the blend state
    const Resource::Id& GetBlendState() const;
    /// set mesh resource
    void SetMesh(const Resource::Id& mesh);
    /// get mesh resource
    const Resource::Id& GetMesh() const;
    /// set the shader program resource
    void SetProgram(const Resource::Id& prog, uint32 selectionMask);
    /// get the program object
    const Resource::Id& GetProgram() const;
    /// get the program selection mask
    uint32 GetProgSelMask() const;
    
    /// get the resource locator
    const Resource::Locator& GetLocator() const;
    
private:
    Resource::Locator loc;
    Resource::Id depthStencilState;
    Resource::Id blendState;
    Resource::Id mesh;
    Resource::Id prog;
    uint32 progSelMask;
};

//------------------------------------------------------------------------------
inline const Resource::Locator&
DrawStateSetup::GetLocator() const {
    return this->loc;
}
    
//------------------------------------------------------------------------------
inline const Resource::Id&
DrawStateSetup::GetDepthStencilState() const {
    return this->depthStencilState;
}

//------------------------------------------------------------------------------
inline const Resource::Id&
DrawStateSetup::GetBlendState() const {
    return this->blendState;
}

//------------------------------------------------------------------------------
inline const Resource::Id&
DrawStateSetup::GetMesh() const {
    return this->mesh;
}

//------------------------------------------------------------------------------
inline const Resource::Id&
DrawStateSetup::GetProgram() const {
    return this->prog;
}

//------------------------------------------------------------------------------
inline uint32
DrawStateSetup::GetProgSelMask() const {
    return this->progSelMask;
}

} // namespace Render
} // namespace Oryol
