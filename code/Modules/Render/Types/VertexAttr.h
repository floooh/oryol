#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::VertexAttr
    @brief vertex attribut enum (position, texcoord, ...)
    
    The VertexAttr definitions don't have a hardwired meaning, they just
    exist to make the binding of vertex components (living in vertex buffers)
    to vertex attribute definition in vertex shaders easier to understand.
    The maximum number of vertex attributes should not exceed 16
    (this is the GL_MAX_VERTEX_ATTRIBS value).
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
    
class VertexAttr {
public:
    /// vertex attribute enum
    enum Code {
        Position = 0,   ///> "position"
        Normal,         ///> "normal"
        TexCoord0,      ///> "texcoord0"
        TexCoord1,      ///> "texcoord1"
        TexCoord2,      ///> "texcoord2"
        TexCoord3,      ///> "texcoord3"
        Tangent,        ///> "tangent
        Binormal,       ///> "binormal"
        Weights,        ///> "weights" (skin weights)
        Indices,        ///> "indices" (skin indices)
        Color0,         ///> "color0"
        Color1,         ///> "color1"
        Custom0,        ///> "custom0"
        Custom1,        ///> "custom1"
        Custom2,        ///> "custom2"
        Custom3,        ///> "custom3"
        
        NumVertexAttrs,
        InvalidVertexAttr,
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol
 