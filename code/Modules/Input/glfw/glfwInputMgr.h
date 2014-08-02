#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::glfwInputMgr
    @brief provide input on platforms using GLFW
*/
#include "Input/base/inputMgrBase.h"

namespace Oryol {
namespace Input {

class glfwInputMgr : public inputMgrBase {
public:
    /// constructor
    glfwInputMgr();
    /// destructor
    ~glfwInputMgr();
    
private:
    /// setup glfw input callbacks
    void setupGlfwCallbacks();
    /// discard glfw input callbacks
    void discardGlfwCallbacks();
};

} // namespace Input
} // namespace Oryol