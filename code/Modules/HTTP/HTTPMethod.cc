//------------------------------------------------------------------------------
//  HTTPMethod.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "HTTPMethod.h"
#include "Core/Macros.h"
#include "Core/Assert.h"
#include <cstring>

namespace Oryol {
namespace HTTP {

//------------------------------------------------------------------------------
const char*
HTTPMethod::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Options);
        __ORYOL_TOSTRING(Get);
        __ORYOL_TOSTRING(Head);
        __ORYOL_TOSTRING(Post);
        __ORYOL_TOSTRING(Put);
        __ORYOL_TOSTRING(Delete);
        __ORYOL_TOSTRING(Trace);
        __ORYOL_TOSTRING(Connect);
        default: return "InvalidHTTPMethod";
    }
}

//------------------------------------------------------------------------------
const wchar_t*
HTTPMethod::ToWideString(Code c) {
    switch (c) {
        case Options: return L"Options";
        case Get: return L"Get";
        case Head: return L"Head";
        case Post: return L"Post";
        case Put: return L"Put";
        case Delete: return L"Delete";
        case Trace: return L"Trace";
        case Connect: return L"Connect";
        default: return L"InvalidHTTPMethod";
    }
}

//------------------------------------------------------------------------------
HTTPMethod::Code
HTTPMethod::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Options);
    __ORYOL_FROMSTRING(Get);
    __ORYOL_FROMSTRING(Head);
    __ORYOL_FROMSTRING(Post);
    __ORYOL_FROMSTRING(Put);
    __ORYOL_FROMSTRING(Delete);
    __ORYOL_FROMSTRING(Trace);
    __ORYOL_FROMSTRING(Connect);
    return InvalidHTTPMethod;
}

} // namespace HTTP
} // namespace Oryol
