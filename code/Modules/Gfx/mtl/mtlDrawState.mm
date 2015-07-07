//------------------------------------------------------------------------------
//  mtlDrawState.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlDrawState.h"
#include "mtl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlDrawState::mtlDrawState() :
mtlRenderPipelineState(nil),
mtlDepthStencilState(nil) {
    // empty
}

//------------------------------------------------------------------------------
mtlDrawState::~mtlDrawState() {
    o_assert_dbg(nil == this->mtlRenderPipelineState);
    o_assert_dbg(nil == this->mtlDepthStencilState);
}

//------------------------------------------------------------------------------
void
mtlDrawState::Clear() {
    this->mtlRenderPipelineState = nil;
    this->mtlDepthStencilState = nil;
    drawStateBase::Clear();
}

} // namespace _priv
} // namespace Oryol