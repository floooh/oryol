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
        Construct,
        EnqueuePreload,
        Preloading,
        Init,
        Running,
        Cleanup,
        Destroy,
        Blocked,
        
        NumAppStates,
        InvalidAppState,
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Oryol
