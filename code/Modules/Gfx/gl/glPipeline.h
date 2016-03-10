#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glPipeline
    @ingroup _priv
    @brief GL implementation of pipeline
*/
#include "Gfx/Resource/pipelineBase.h"
#include "Gfx/gl/glVertexAttr.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {

class glPipeline : public pipelineBase {
public:
    /// constructor
    glPipeline();
    /// clear the object (called from pipelineFactory::DestroyResource()
    void Clear();
    
    StaticArray<glVertexAttr, VertexAttr::NumVertexAttrs> glAttrs;
    GLenum glPrimType;
};

} // namespace _priv
} // namespace Oryol
