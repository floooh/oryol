#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::VertexFormat
    @brief vertex component formats
    @see VertexComponent, VertexLayout
    
    Describes the data type of a vertex component.
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {

class VertexFormat {
public:
    /// format enum
    enum Code {
        Float = 0,      ///> single component float, expanded to (x, 0, 0, 1)
        Float2,         ///> 2-component float, expanded to (x, y, 0, 1)
        Float3,         ///> 3-component float, expanded to (x, y, z, 1)
        Float4,         ///> 4-component float
        UByte4,         ///> 4-component float, 0..255
        UByte4N,        ///> 4-component float normalized (value / 255.0)
        Short2,         ///> 2-component signed short
        Short2N,        ///> 2-component signed short, normalized (value / 32767.0)
        Short4,         ///> 4-component signed short
        Short4N,        ///> 4-component signed short, normalized (value / 32767.0)
        
        NumVertexFormats,       ///> number of vertex formats
        InvalidVertexFormat,    ///> the invalid vertex format value
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
    /// get the byte size of a vertex format code
    static int32 ByteSize(Code c);
};

} // namespace Render
} // namespace Oryol