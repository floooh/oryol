//------------------------------------------------------------------------------
//  VertexFormat.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexFormat.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
VertexFormat::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Float);
        __ORYOL_TOSTRING(Float2);
        __ORYOL_TOSTRING(Float3);
        __ORYOL_TOSTRING(Float4);
        __ORYOL_TOSTRING(UByte4);
        __ORYOL_TOSTRING(UByte4N);
        __ORYOL_TOSTRING(Short2);
        __ORYOL_TOSTRING(Short2N);
        __ORYOL_TOSTRING(Short4);
        __ORYOL_TOSTRING(Short4N);
        default: return "InvalidVertexFormat";
    }
}

//------------------------------------------------------------------------------
VertexFormat::Code
VertexFormat::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Float);
    __ORYOL_FROMSTRING(Float2);
    __ORYOL_FROMSTRING(Float3);
    __ORYOL_FROMSTRING(Float4);
    __ORYOL_FROMSTRING(UByte4);
    __ORYOL_FROMSTRING(UByte4N);
    __ORYOL_FROMSTRING(Short2);
    __ORYOL_FROMSTRING(Short2N);
    __ORYOL_FROMSTRING(Short4);
    __ORYOL_FROMSTRING(Short4N);
    return InvalidVertexFormat;
}

//------------------------------------------------------------------------------
int32
VertexFormat::ByteSize(Code c) {
    switch (c) {
        case Float:
            return 4;
        case Float2:
            return 8;
        case Float3:
            return 12;
        case Float4:
            return 16;
        case UByte4:
        case UByte4N:
        case Short2:
        case Short2N:
            return 4;
        case Short4:
        case Short4N:
            return 8;
        default:
            o_error("VertexFormat::ByteSize() called with invalid format!\n");
            return 0;
    }
}


} // namespace Render
} // namespace Oryol