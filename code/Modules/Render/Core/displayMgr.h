#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::displayMgr
    @brief private: create and manage the app window and GL context
    
    The display manager object manages application window and
    GL context creation, and usually processes host window system
    events (such as input events) and forwards them to Oryol.
*/
#if (ORYOL_WINDOWS || ORYOL_MACOS || ORYOL_LINUX)
#include "Render/glfw/glfwDisplayMgr.h"
namespace Oryol {
namespace Render {
class displayMgr : public glfwDisplayMgr {
    // empty
};
} }
#elif (ORYOL_EMSCRIPTEN || ORYOL_ANDROID)
#include "Render/egl/eglDisplayMgr.h"
namespace Oryol {
namespace Render {
class displayMgr : public eglDisplayMgr {
    // empty;
};
} }
#elif ORYOL_IOS
#include "Render/ios/iosDisplayMgr.h"
namespace Oryol {
namespace Render {
class displayMgr : public iosDisplayMgr {
    // empty
};
} }
#elif ORYOL_PNACL
#include "Render/pnacl/pnaclDisplayMgr.h"
namespace Oryol {
namespace Render {
class displayMgr : public pnaclDisplayMgr {
    // empty
};
} }
#else
#include "Render/base/displayMgrBase.h"
namespace Oryol {
namespace Render {
class displayMgr : public displayMgrBase {
    // empty
};
} }
#endif
 