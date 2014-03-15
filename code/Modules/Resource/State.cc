//------------------------------------------------------------------------------
//  State.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "State.h"
#include "Core/Macros.h"
#include "Core/Assert.h"
#include <cstring>

namespace Oryol {
namespace Resource {
    
//------------------------------------------------------------------------------
const char*
State::ToString(Code c) {
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
State::Code
State::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Initial);
    __ORYOL_FROMSTRING(Setup);
    __ORYOL_FROMSTRING(Pending);
    __ORYOL_FROMSTRING(Valid);
    __ORYOL_FROMSTRING(Failed);
    return InvalidState;
}

} // namespace Render
} // namespace Oryol