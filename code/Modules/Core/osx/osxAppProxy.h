#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::osxAppProxy
    @ingroup _priv
    @brief global application wrapper for OSX/IOS apps
    
    NOTE: this wrapper is only used with Automatic Reference Counting,
    and when not using GLFW.
*/
#include "Core/Types.h"

namespace Oryol {
class App;

namespace _priv {

class osxAppProxy {
public:
    /// called to initialize the application
    static void init();
    /// called to terminate the application
    static void terminate();
    /// called to invoke the App onFrame method (wrapped in an @autoreleasepool)
    static void onFrame(App* app);
};

} // namespace _priv
} // namespace Oryol