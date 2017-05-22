#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::inputMgr
    @ingroup _priv
    @brief frontend inputMgr class
*/
#if ORYOL_D3D11
#include "Input/private/win/winInputMgr.h"
namespace Oryol {
namespace _priv {
class inputMgr : public winInputMgr { };
} }
#elif ORYOL_MACOS && ORYOL_METAL
#include "Input/private/osx/osxInputMgr.h"
namespace Oryol {
namespace _priv {
class inputMgr : public osxInputMgr { };
} }
#elif ORYOL_RASPBERRYPI
#include "Input/private/raspi/raspiInputMgr.h"
namespace Oryol {
namespace _priv {
class inputMgr : public raspiInputMgr { };
} }
#elif (ORYOL_WINDOWS || ORYOL_MACOS || ORYOL_LINUX)
#include "Input/private/glfw/glfwInputMgr.h"
namespace Oryol {
namespace _priv {
class inputMgr : public glfwInputMgr { };
} }
#elif ORYOL_EMSCRIPTEN
#include "Input/private/emsc/emscInputMgr.h"
namespace Oryol {
namespace _priv {
class inputMgr : public emscInputMgr { };
} }
#elif ORYOL_ANDROID
#include "Input/private/android/androidInputMgr.h"
namespace Oryol {
namespace _priv {
class inputMgr : public androidInputMgr { };
} }
#elif ORYOL_IOS
#include "Input/private/ios/iosInputMgr.h"
namespace Oryol {
namespace _priv {
class inputMgr : public iosInputMgr { };
} }
#else
#include "Input/private/inputMgrBase.h"
namespace Oryol {
namespace _priv {
class inputMgr : public inputMgrBase { };
} }
#endif
