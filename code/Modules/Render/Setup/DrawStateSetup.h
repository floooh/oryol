#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::DrawStateSetup
    @ingroup Render
    @brief setup object for DrawState resources
*/
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Render/Core/BlendState.h"
#include "Render/Core/DepthStencilState.h"
#include "Render/Core/RasterizerState.h"

namespace Oryol {

class DrawStateSetup {
public:
    /// default constructor
    DrawStateSetup();
    /// construct with locator
    DrawStateSetup(const Locator& loc);
    /// construct with locator and resources
    DrawStateSetup(const Locator& loc, const Id& mesh, const Id& prog, uint32 progSelMask);
    
    /// resource locator
    class Locator Locator;
    /// blend state
    class BlendState BlendState;
    /// depth-stencil state
    class DepthStencilState DepthStencilState;
    /// rasterizer state
    class RasterizerState RasterizerState;
    /// mesh
    Id Mesh;
    /// program bundle
    Id Program;
    /// program bundle selection mask
    uint32 ProgramSelectionMask{0};
};

} // namespace Oryol
