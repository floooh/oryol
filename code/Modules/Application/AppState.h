#pragma once
//------------------------------------------------------------------------------
/**
    @class Application::AppState
    @brief application states
*/
#include "Core/Types.h"

namespace Oryol {
namespace Application {
    
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
        
        NumAppStates,
        InvalidAppState,
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Application
} // namespace Oryol
