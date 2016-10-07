#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::PipelineSetup
    @ingroup Gfx
    @brief setup object for pipeline resources
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

class PipelineSetup {
public:
    /// construct from shader
    static PipelineSetup FromShader(const Id& shd);
    /// construct from vertex layout and shader
    static PipelineSetup FromLayoutAndShader(const VertexLayout& layout, const Id& shd);

    /// default constructor
    PipelineSetup();

    /// resource locator
    class Locator Locator;
    /// blend state (GLES3.0 doesn't allow separate MRT blend state
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
    /// enable vertex capture?
    bool EnableVertexCapture = false;
    /// vertex layout of vertex capture mesh
    VertexLayout CaptureLayout;
};

} // namespace Oryol
