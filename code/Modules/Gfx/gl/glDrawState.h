#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glDrawState
    @ingroup _priv
    @brief GL implementation of drawState
*/
#include "Gfx/Resource/drawStateBase.h"
#include "Gfx/gl/glVertexAttr.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {

class glDrawState : public drawStateBase {
public:
    /// constructor
    glDrawState();
    /// clear the object (called from drawStateFactory::DestroyResource()
    void Clear();
    
    StaticArray<glVertexAttr, VertexAttr::NumVertexAttrs> glAttrs;
    GLenum glPrimType;
};

} // namespace _priv
} // namespace Oryol