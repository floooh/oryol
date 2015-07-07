#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlDrawState
    @ingroup _priv
    @brief Metal implementation of class drawState
*/
#include "Gfx/Resource/drawStateBase.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class mtlDrawState : public drawStateBase {
public:
    /// constructor
    mtlDrawState();
    /// destructor
    ~mtlDrawState();

    /// clear the object (called from drawStateFactory::DestroyResource())
    void Clear();

    /// render-pipeline state
    ORYOL_OBJC_TYPED_ID(MTLRenderPipelineState) mtlRenderPipelineState;
    /// depth-stencil state
    ORYOL_OBJC_TYPED_ID(MTLDepthStencilState) mtlDepthStencilState;
};

} // namespace _priv
} // namespace Oryol