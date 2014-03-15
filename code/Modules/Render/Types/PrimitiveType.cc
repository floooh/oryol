//------------------------------------------------------------------------------
//  PrimitiveType.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "PrimitiveType.h"
#include "Core/Assert.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
PrimitiveType::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Points);
        __ORYOL_TOSTRING(LineStrip);
        __ORYOL_TOSTRING(LineLoop);
        __ORYOL_TOSTRING(Lines);
        __ORYOL_TOSTRING(TriangleStrip);
        __ORYOL_TOSTRING(TriangleFan);
        __ORYOL_TOSTRING(Triangles);
        default: return "InvalidPrimitiveType";
    }
}

//------------------------------------------------------------------------------
PrimitiveType::Code
PrimitiveType::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Points);
    __ORYOL_FROMSTRING(LineStrip);
    __ORYOL_FROMSTRING(LineLoop);
    __ORYOL_FROMSTRING(Lines);
    __ORYOL_FROMSTRING(TriangleStrip);
    __ORYOL_FROMSTRING(TriangleFan);
    __ORYOL_FROMSTRING(Triangles);
    return InvalidPrimitiveType;
}

} // namespace Render
} // namespace Oryol