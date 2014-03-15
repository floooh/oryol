//------------------------------------------------------------------------------
//  Usage.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Usage.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
Usage::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Immutable);
        __ORYOL_TOSTRING(DynamicWrite);
        __ORYOL_TOSTRING(DynamicStream);
        default: return "InvalidUsage";
    }
}

//------------------------------------------------------------------------------
Usage::Code
Usage::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Immutable);
    __ORYOL_FROMSTRING(DynamicWrite);
    __ORYOL_FROMSTRING(DynamicStream);
    return InvalidUsage;
}

} // namespace Render
} // namespace Oryol