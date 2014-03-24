//------------------------------------------------------------------------------
//  AppState.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "AppState.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Application {
    
//------------------------------------------------------------------------------
const char*
AppState::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Construct);
        __ORYOL_TOSTRING(EnqueuePreload);
        __ORYOL_TOSTRING(Preloading);
        __ORYOL_TOSTRING(Init);
        __ORYOL_TOSTRING(Running);
        __ORYOL_TOSTRING(Cleanup);
        __ORYOL_TOSTRING(Destroy);
        default: return "InvalidAppState";
    }
}

//------------------------------------------------------------------------------
AppState::Code
AppState::FromString(const char* str) {
    __ORYOL_FROMSTRING(Construct);
    __ORYOL_FROMSTRING(EnqueuePreload);
    __ORYOL_FROMSTRING(Preloading);
    __ORYOL_FROMSTRING(Init);
    __ORYOL_FROMSTRING(Running);
    __ORYOL_FROMSTRING(Cleanup);
    __ORYOL_FROMSTRING(Destroy);
    return AppState::InvalidAppState;
}

} // namespace Application
} // namespace Oryol