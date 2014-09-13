#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::displayMgr
    @ingroup _priv
    @brief private: create and manage the app window and GL context
    
    The display manager object manages application window and
    GL context creation, and usually processes host window system
    events (such as input events) and forwards them to Oryol.
*/
#if (ORYOL_WINDOWS || ORYOL_MACOS || ORYOL_LINUX)
#include "Render/glfw/glfwDisplayMgr.h"
namespace Oryol {
namespace _priv {
class displayMgr : public glfwDisplayMgr { };
} }
#elif ORYOL_EMSCRIPTEN
#include "Render/emsc/emscDisplayMgr.h"
namespace Oryol {
namespace _priv {
class displayMgr : public emscDisplayMgr { };
} }
#elif ORYOL_ANDROID
#include "Render/egl/eglDisplayMgr.h"
namespace Oryol {
namespace _priv {
class displayMgr : public eglDisplayMgr { };
} }
#elif ORYOL_IOS
#include "Render/ios/iosDisplayMgr.h"
namespace Oryol {
namespace _priv {
class displayMgr : public iosDisplayMgr { };
} }
#elif ORYOL_PNACL
#include "Render/pnacl/pnaclDisplayMgr.h"
namespace Oryol {
namespace _priv {
class displayMgr : public pnaclDisplayMgr { };
} }
#else
#include "Render/base/displayMgrBase.h"
namespace Oryol {
namespace _priv {
class displayMgr : public displayMgrBase { };
} }
#endif
 