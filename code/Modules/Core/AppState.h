#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::AppState
    @brief application states
*/
#include "Core/Types.h"

namespace Oryol {
namespace Core {
    
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
    
} // namespace Core
} // namespace Oryol
