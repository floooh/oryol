#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::Mesh
    @brief geometry mesh for rendering
    
    A Mesh object holds one or more vertex buffers, an optional 
    index buffer, and one or more primitive groups.
    @todo: describe mesh creation etc...
*/
#if ORYOL_OPENGL
#include "Render/gl/glMesh.h"
namespace Oryol {
namespace Render {
class mesh : public glMesh { };
} }
#else
#error "Target platform not yet supported!"
#endif
