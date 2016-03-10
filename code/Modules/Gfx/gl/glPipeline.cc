//------------------------------------------------------------------------------
//  glPipeline.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glPipeline.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glPipeline::glPipeline() :
glPrimType(0) {
    // empty
}

//------------------------------------------------------------------------------
void
glPipeline::Clear() {
    this->glAttrs.Fill(glVertexAttr());
    this->glPrimType = 0;
    pipelineBase::Clear();
}

} // namespace _priv
} // namespace Oryol