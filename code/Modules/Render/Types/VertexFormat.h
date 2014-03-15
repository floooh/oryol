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
    /// format enum (don't change order, and append to end!)
    enum Code {
        Float,          ///< single component float, expanded to (x, 0, 0, 1)
        Float2,         ///< 2-component float, expanded to (x, y, 0, 1)
        Float3,         ///< 3-component float, expanded to (x, y, z, 1)
        Float4,         ///< 4-component float
        Byte4,          ///< 4-component float (-128.0f..+127.0f) mapped to byte (-128..+127)
        Byte4N,         ///< 4-component float (-1.0f..+1.0f) mapped to byte (-128..+127)
        UByte4,         ///< 4-component float (0.0f..255.0f) mapped to byte (0..255)
        UByte4N,        ///< 4-component float (0.0f..+1.0) mapped to byte (-128..+127)
        Short2,         ///< 2-component float (-32768.0f..+32767.0f) mapped to short (-32768..+32768)
        Short2N,        ///< 2-component float (-1.0f..+1.0f) mapped to short (-32768..+32768)
        Short4,         ///< 4-component float (-32768.0f..+32767.0f) mapped to short (-32768..+32768)
        Short4N,        ///< 4-component float (-1.0f..+1.0f) mapped to short (-32768..+32768)
        
        NumVertexFormats,       ///< number of vertex formats
        InvalidVertexFormat,    ///< the invalid vertex format value
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