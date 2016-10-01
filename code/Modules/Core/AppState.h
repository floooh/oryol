#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::AppState
    @ingroup Core
    @brief application states
*/
#include "Core/Types.h"

namespace Oryol {
    
class AppState {
public:
    /// app state enum
    enum Code {
        Init,
        Running,
        Cleanup,
        Destroy,
        Blocked,
        
        NumAppStates,
        InvalidAppState,
    };
    
    /// convert to string
    static const char* ToString(Code c) {
        switch (c) {
            case Init:      return "Init";
            case Cleanup:   return "Cleanup";
            case Destroy:   return "Destroy";
            case Blocked:   return "Blocked";
            default: return "InvalidAppState";
        }
    };
};
    
} // namespace Oryol
