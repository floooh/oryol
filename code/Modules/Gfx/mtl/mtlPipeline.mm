//------------------------------------------------------------------------------
//  mtlPipeline.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlPipeline.h"
#include "mtl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlPipeline::mtlPipeline() :
mtlRenderPipelineState(nil),
mtlDepthStencilState(nil) {
    // empty
}

//------------------------------------------------------------------------------
mtlPipeline::~mtlPipeline() {
    o_assert_dbg(nil == this->mtlRenderPipelineState);
    o_assert_dbg(nil == this->mtlDepthStencilState);
}

//------------------------------------------------------------------------------
void
mtlPipeline::Clear() {
    this->mtlRenderPipelineState = nil;
    this->mtlDepthStencilState = nil;
    pipelineBase::Clear();
}

} // namespace _priv
} // namespace Oryol