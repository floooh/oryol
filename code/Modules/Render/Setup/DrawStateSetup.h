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
    DrawStateSetup(const Resource::Locator& loc, const Resource::Id& mesh, const Resource::Id& prog, uint32 progSelMask);
    
    /// resource locator
    Resource::Locator Locator;
    /// blend state
    class BlendState BlendState;
    /// depth-stencil state
    class DepthStencilState DepthStencilState;
    /// rasterizer state
    class RasterizerState RasterizerState;
    /// mesh
    Resource::Id Mesh;
    /// program bundle
    Resource::Id Program;
    /// program bundle selection mask
    uint32 ProgramSelectionMask{0};
};

} // namespace Render
} // namespace Oryol
