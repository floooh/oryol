#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glDrawStateFactory
    @ingroup _priv
    @brief GL implementation of drawStateFactory
*/
#include "Gfx/Resource/drawStateFactoryBase.h"
#include "Gfx/gl/gl_decl.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
namespace _priv {

class glDrawStateFactory : public drawStateFactoryBase {
public:
    /// setup drawState resource
    ResourceState::Code SetupResource(drawState& ds);
    /// destroy the drawState
    void DestroyResource(drawState& ds);

private:
    /// setup the GL vertex attribute declaration
    void glSetupVertexAttrs(drawState& ds);

    #if ORYOL_GL_USE_VERTEXARRAYOBJECT
    /// setup GL vertex array object
    void glSetupVertexArrayObject(drawState& ds);
    #endif

    /// table to convert VertexFormat to GL vertex attr size, type, normalized
    static const struct VertexFormatTable {
        GLint size;
        GLenum type;
        GLboolean normalized;
    } vertexFormatTable[VertexAttr::NumVertexAttrs];
};

} // namespace _priv
} // namespace Oryol