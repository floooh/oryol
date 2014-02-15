//------------------------------------------------------------------------------
//  HTTPMethod.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPMethod.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace HTTP {

//------------------------------------------------------------------------------
const char*
HTTPMethod::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(OPTIONS);
        __ORYOL_TOSTRING(GET);
        __ORYOL_TOSTRING(HEAD);
        __ORYOL_TOSTRING(POST);
        __ORYOL_TOSTRING(PUT);
        __ORYOL_TOSTRING(DELETE);
        __ORYOL_TOSTRING(TRACE);
        __ORYOL_TOSTRING(CONNECT);
        default: return "InvalidHTTPMethod";
    }
}

//------------------------------------------------------------------------------
HTTPMethod::Code
HTTPMethod::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(OPTIONS);
    __ORYOL_FROMSTRING(GET);
    __ORYOL_FROMSTRING(HEAD);
    __ORYOL_FROMSTRING(POST);
    __ORYOL_FROMSTRING(PUT);
    __ORYOL_FROMSTRING(DELETE);
    __ORYOL_FROMSTRING(TRACE);
    __ORYOL_FROMSTRING(CONNECT);
    return InvalidHTTPMethod;
}

} // namespace HTTP
} // namespace Oryol