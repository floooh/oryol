#pragma once
//------------------------------------------------------------------------------
/**
    @class Input::inputMgr
    @brief frontend inputMgr class
*/
#if (ORYOL_WINDOWS || ORYOL_MACOS || ORYOL_LINUX)
#include "Input/glfw/glfwInputMgr.h"
namespace Oryol {
namespace Input {
class inputMgr : public glfwInputMgr { };
} }
#elif ORYOL_EMSCRIPTEN
#include "Input/emsc/emscInputMgr.h"
namespace Oryol {
namespace Input {
class inputMgr : public emscInputMgr { };
} }
#else
#include "Input/base/inputMgrBase.h"
namespace Oryol {
namespace Input {
class inputMgr : public inputMgrBase { };
} }
#endif
