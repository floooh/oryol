#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::PrimitiveType
    @brief primitive type enum (triangle strips, lists, etc...)
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
    
class PrimitiveType {
public:
    /// primitive type enum (don't change order, append to end!)
    enum Code {
        Points = 0,             ///< point list
        LineStrip,              ///< line strip
        LineLoop,               ///< closed line loop
        Lines,                  ///< line list
        TriangleStrip,          ///< triangle strip
        TriangleFan,            ///< triangle fan
        Triangles,              ///< triangle list
        
        NumPrimitiveTypes,      ///< number of primitive types
        InvalidPrimitiveType,   ///< invalid primitive type value
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol