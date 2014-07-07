#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::DrawStateSetup
    @brief setup object for DrawState resources
*/
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Render/Core/BlendState.h"
#include "Render/Core/DepthStencilState.h"
#include "Render/Core/RasterizerState.h"

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
                   const Resource::Id& mesh,
                   const Resource::Id& prog, uint32 progSelMask);
    
    /// read/write access to embedded blend state
    class BlendState& BlendState();
    /// read-only access to embedded blend state
    const class BlendState& BlendState() const;
    /// read/write acess to embedded depth-stencil state
    class DepthStencilState& DepthStencilState();
    /// read-only access to embedded depth-stencil satte
    const class DepthStencilState& DepthStencilState() const;
    /// read/write access to rasterizer state
    class RasterizerState& RasterizerState();
    /// read-only access to rasterizer state
    const class RasterizerState& RasterizerState() const;
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
    class BlendState blendState;
    class DepthStencilState depthStencilState;
    class RasterizerState rasterizerState;
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
inline BlendState&
DrawStateSetup::BlendState() {
    return this->blendState;
}

//------------------------------------------------------------------------------
inline const BlendState&
DrawStateSetup::BlendState() const {
    return this->blendState;
}

//------------------------------------------------------------------------------
inline DepthStencilState&
DrawStateSetup::DepthStencilState() {
    return this->depthStencilState;
}

//------------------------------------------------------------------------------
inline const DepthStencilState&
DrawStateSetup::DepthStencilState() const {
    return this->depthStencilState;
}

//------------------------------------------------------------------------------
inline RasterizerState&
DrawStateSetup::RasterizerState() {
    return this->rasterizerState;
}

//------------------------------------------------------------------------------
inline const RasterizerState&
DrawStateSetup::RasterizerState() const {
    return this->rasterizerState;
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
