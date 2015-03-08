#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::DrawStateSetup
    @ingroup Gfx
    @brief setup object for DrawState resources
*/
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Gfx/Core/BlendState.h"
#include "Gfx/Core/DepthStencilState.h"
#include "Gfx/Core/RasterizerState.h"

namespace Oryol {

class DrawStateSetup {
public:
    /// construct from mesh and prog
    static DrawStateSetup FromMeshAndProg(const Id& mesh, const Id& prog, uint32 progSelMask=0);

    /// default constructor
    DrawStateSetup();
    
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
    uint32 ProgramSelectionMask;
};

} // namespace Oryol
