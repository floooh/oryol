#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlPipeline
    @ingroup _priv
    @brief Metal implementation of class pipeline
*/
#include "Gfx/Resource/pipelineBase.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class mtlPipeline : public pipelineBase {
public:
    /// constructor
    mtlPipeline();
    /// destructor
    ~mtlPipeline();

    /// clear the object (called from pipelineFactory::DestroyResource())
    void Clear();

    /// render-pipeline state
    ORYOL_OBJC_TYPED_ID(MTLRenderPipelineState) mtlRenderPipelineState;
    /// depth-stencil state
    ORYOL_OBJC_TYPED_ID(MTLDepthStencilState) mtlDepthStencilState;
};

} // namespace _priv
} // namespace Oryol
