//------------------------------------------------------------------------------
//  State.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ResourceState.h"
#include "Core/Macros.h"
#include "Core/Assertion.h"
#include <cstring>

namespace Oryol {
    
//------------------------------------------------------------------------------
const char*
ResourceState::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Initial);
        __ORYOL_TOSTRING(Setup);
        __ORYOL_TOSTRING(Pending);
        __ORYOL_TOSTRING(Valid);
        __ORYOL_TOSTRING(Failed);
        default: return "InvalidState";
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
ResourceState::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Initial);
    __ORYOL_FROMSTRING(Setup);
    __ORYOL_FROMSTRING(Pending);
    __ORYOL_FROMSTRING(Valid);
    __ORYOL_FROMSTRING(Failed);
    return InvalidState;
}

} // namespace Oryol