#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::glfwInputMgr
    @brief provide input on platforms using GLFW
*/
#include "Input/base/inputMgrBase.h"

struct GLFWwindow;

namespace Oryol {
namespace Input {

class glfwInputMgr : public inputMgrBase {
public:
    /// constructor
    glfwInputMgr();
    /// destructor
    ~glfwInputMgr();
    
    /// set mouse cursor mode
    void setCursorMode(CursorMode::Code mode);
    
private:
    /// per-frame callback to reset input state at start of frame
    void reset();

    /// setup the key mapping table
    void setupKeyTable();
    /// setup glfw input callbacks
    void setupCallbacks(GLFWwindow* glfwWindow);
    /// discard glfw input callbacks
    void discardCallbacks(GLFWwindow* glfwWindow);
    /// map GLFW key code to Oryol key code
    Key::Code mapKey(int glfwKey) const;
    
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
};

} // namespace Input
} // namespace Oryol