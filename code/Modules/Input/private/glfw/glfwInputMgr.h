#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glfwInputMgr
    @ingroup _priv
    @brief provide input on platforms using GLFW
*/
#include "Input/private/inputMgrBase.h"
#include "Core/Containers/Map.h"

struct GLFWwindow;

namespace Oryol {
namespace _priv {

class glfwInputMgr : public inputMgrBase {
public:
    /// constructor
    glfwInputMgr();
    /// destructor
    ~glfwInputMgr();
    
    /// setup the GLFW input manager
    void setup(const InputSetup& setup);
    /// discard the GLFW input manager
    void discard();
    
    /// setup the key mapping table
    void setupKeyTable();
    /// setup glfw input callbacks
    void setupCallbacks(GLFWwindow* glfwWindow);
    /// discard glfw input callbacks
    void discardCallbacks(GLFWwindow* glfwWindow);
    /// map GLFW key code to Oryol key code
    Key::Code mapKey(int glfwKey) const;

    /// setup game pad mappings for known gamepads
    void setupGamepadMappings();
    /// update gamepad state
    void updateGamepads(); 
    /// GLFW key callback
    static void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods);
    /// GLFW char callback
    static void charCallback(GLFWwindow* win, unsigned int unicode);
    /// GLFW mouse button callback
    static void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods);
    /// GLFW cursor position callback
    static void cursorPosCallback(GLFWwindow* win, double x, double y);
    /// GLFW cursor enter callback
    static void cursorEnterCallback(GLFWwindow* win, int entered);
    /// GLFW scroll callback
    static void scrollCallback(GLFWwindow* win, double xOffset, double yOffset);
    
    static glfwInputMgr* self;
    int resetRunLoopId;
    int updateGamepadsRunLoopId;
};

} // namespace _priv
} // namespace Oryol
