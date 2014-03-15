#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::Mesh
    @brief geometry mesh for rendering
    
    A Mesh object holds one or more vertex buffers, an optional 
    index buffer, and one or more primitive groups.
    @todo: describe mesh creation etc...
*/
// only GL platforms for now
#include "Render/gl/glMesh.h"
namespace Oryol {
namespace Render {
class Mesh : public glMesh { };
};
} }
