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
    /// primitive type enum
    enum Code {
        Points = 0,
        LineStrip,
        LineLoop,
        Lines,
        TriangleStrip,
        TriangleFan,
        Triangles,
        
        NumPrimitiveTypes,
        InvalidPrimitiveType,
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol