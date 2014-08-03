#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::glfwInputMgr
    @brief provide input on platforms using GLFW
*/
#include "Input/base/inputMgrBase.h"
#include "Input/Core/CursorMode.h"

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
    /// get mouse cursor mode
    CursorMode::Code getCursorMode() const;
    
private:
    /// per-frame callback to reset input state at start of frame
    void reset();

    /// setup the key mapping table
    void setupKeyTable();
    /// setup glfw input callbacks
    void setupGlfwCallbacks(GLFWwindow* glfwWindow);
    /// discard glfw input callbacks
    void discardGlfwCallbacks();
    /// map GLFW key code to Oryol key code
    Key::Code mapKey(int glfwKey);
    
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
    CursorMode::Code cursorMode;
};

} // namespace Input
} // namespace Oryol