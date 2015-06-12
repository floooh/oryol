#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::DrawStateSetup
    @ingroup Gfx
    @brief setup object for DrawState resources
*/
#include "Core/Containers/StaticArray.h"
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Gfx/Core/BlendState.h"
#include "Gfx/Core/DepthStencilState.h"
#include "Gfx/Core/RasterizerState.h"
#include "glm/vec4.hpp"

namespace Oryol {

class DrawStateSetup {
public:
    /// construct from mesh and prog
    static DrawStateSetup FromMeshAndProg(const Id& mesh, const Id& prog, uint32 progSelMask=0);
    /// max number of input meshes
    static const int32 MaxInputMeshes = 4;

    /// default constructor
    DrawStateSetup();

    /// resource locator
    class Locator Locator;
    /// blend state
    class BlendState BlendState;
    /// blend color
    glm::vec4 BlendColor;
    /// depth-stencil state
    class DepthStencilState DepthStencilState;
    /// rasterizer state
    class RasterizerState RasterizerState;
    /// input mesh(es)
    StaticArray<Id, MaxInputMeshes> Meshes;
    /// program bundle
    Id Program;
    /// program bundle selection mask
    uint32 ProgramSelectionMask;
};

} // namespace Oryol
