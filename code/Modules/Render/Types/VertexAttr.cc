//------------------------------------------------------------------------------
//  VertexAttr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexAttr.h"
#include "Core/Assert.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
/**
 NOTE: this is a special case in that the string representation is all
 lower-case, this is because the string must match the vertex attribute
 name in vertex shaders.
*/
const char*
VertexAttr::ToString(Code c) {
    switch (c) {
        case Position:  return "position";
        case Normal:    return "normal";
        case TexCoord0: return "texcoord0";
        case TexCoord1: return "texcoord1";
        case TexCoord2: return "texcoord2";
        case TexCoord3: return "texcoord3";
        case Tangent:   return "tangent";
        case Binormal:  return "binormal";
        case Weights:   return "weights";
        case Indices:   return "indices";
        case Color0:    return "color0";
        case Color1:    return "color1";
        case Custom0:   return "custom0";
        case Custom1:   return "custom1";
        case Custom2:   return "custom2";
        case Custom3:   return "custom3";
        default:
            o_error("VertexAttr::ToString(): invalid value!\n");
            return 0;
    }
}
    
//------------------------------------------------------------------------------
VertexAttr::Code
VertexAttr::FromString(const char* str) {
    o_assert(str);
    if (0 == std::strcmp(str, "position")) return Position;
    if (0 == std::strcmp(str, "normal")) return Normal;
    if (0 == std::strcmp(str, "texcoord0")) return TexCoord0;
    if (0 == std::strcmp(str, "texcoord1")) return TexCoord1;
    if (0 == std::strcmp(str, "texcoord2")) return TexCoord2;
    if (0 == std::strcmp(str, "texcoord3")) return TexCoord3;
    if (0 == std::strcmp(str, "tangent")) return Tangent;
    if (0 == std::strcmp(str, "binormal")) return Binormal;
    if (0 == std::strcmp(str, "weights")) return Weights;
    if (0 == std::strcmp(str, "indices")) return Indices;
    if (0 == std::strcmp(str, "color0")) return Color0;
    if (0 == std::strcmp(str, "color1")) return Color1;
    if (0 == std::strcmp(str, "custom0")) return Custom0;
    if (0 == std::strcmp(str, "custom1")) return Custom1;
    if (0 == std::strcmp(str, "custom2")) return Custom2;
    if (0 == std::strcmp(str, "custom3")) return Custom3;
    else return InvalidVertexAttr;
}
    
} // namespace Render
} // namespace Oryol