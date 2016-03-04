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
#include "Gfx/Core/VertexLayout.h"
#include "Gfx/Core/GfxConfig.h"
#include "glm/vec4.hpp"

namespace Oryol {

class DrawStateSetup {
public:
    /// construct from shader
    static DrawStateSetup FromShader(const Id& shd);
    /// construct from vertex layout and shader
    static DrawStateSetup FromLayoutAndShader(const VertexLayout& layout, const Id& shd);

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
    /// input vertex layouts (one per mesh slot)
    StaticArray<VertexLayout, GfxConfig::MaxNumInputMeshes> Layouts;
    /// primitive type 
    PrimitiveType::Code PrimType;
    /// shader 
    Id Shader;
};

} // namespace Oryol
