#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::displayMgr
    @brief private: create and manage the app window and GL context
    
    The display manager object manages application window and
    GL context creation, and usually processes host window system
    events (such as input events) and forwards them to Oryol.
*/
#if (ORYOL_WINDOWS || ORYOL_OSX || ORYOL_LINUX)
#include "Render/glfw/glfwDisplayMgr.h"
namespace Oryol {
namespace Render {
class displayMgr : public glfwDisplayMgr {
    // empty
};
} }
#else
namespace Oryol {
namespace Render {
class displayMgr : public displayMgrBase {
    // empty
};
} }
#endif
 