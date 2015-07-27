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
#include "Gfx/Core/GfxConfig.h"
#include "glm/vec4.hpp"

namespace Oryol {

class DrawStateSetup {
public:
    /// construct from mesh and prog
    static DrawStateSetup FromMeshAndShader(const Id& mesh, const Id& shd, uint32 shdSelMask=0);

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
    StaticArray<Id, GfxConfig::MaxNumInputMeshes> Meshes;
    /// shader 
    Id Shader;
    /// shader selection mask
    uint32 ShaderSelectionMask;
};

} // namespace Oryol
