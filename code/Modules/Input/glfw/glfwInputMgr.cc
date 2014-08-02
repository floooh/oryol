//------------------------------------------------------------------------------
//  glfwInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glfwInputMgr.h"
#include "Render/glfw/glfwDisplayMgr.h"
#include "GLFW/glfw3.h"

namespace Oryol {
namespace Input {

//------------------------------------------------------------------------------
static void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
    Core::Log::Info("key: %d %d %d %d\n", key, scancode, action, mods);
}

//------------------------------------------------------------------------------
static void charCallback(GLFWwindow* win, unsigned int unicode) {
    Core::Log::Info("char: %d\n", unicode);
}

//------------------------------------------------------------------------------
static void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods) {
    Core::Log::Info("mousebutton: %d %d %d\n", button, action, mods);
}

//------------------------------------------------------------------------------
static void cursorPosCallback(GLFWwindow* win, double x, double y) {
    Core::Log::Info("cursorpos: %f %f\n", x, y);
}

//------------------------------------------------------------------------------
static void cursorEnterCallback(GLFWwindow* win, int entered) {
    Core::Log::Info("cursorenter: %d\n", entered);
}

//------------------------------------------------------------------------------
static void scrollCallback(GLFWwindow* win, double xOffset, double yOffset) {
    Core::Log::Info("scroll: %f %f\n", xOffset, yOffset);
}

//------------------------------------------------------------------------------
glfwInputMgr::glfwInputMgr() {
    this->setupGlfwCallbacks();
}

//------------------------------------------------------------------------------
glfwInputMgr::~glfwInputMgr() {
    this->discardGlfwCallbacks();
}

//------------------------------------------------------------------------------
void
glfwInputMgr::setupGlfwCallbacks() {
    
    // first check that the Render subsystem has already been initialized
    GLFWwindow* glfwWindow = Render::glfwDisplayMgr::getGlfwWindow();
    if (nullptr == glfwWindow) {
        o_error("glfwInputMgr: RenderFacade must be created before InputFacade!");
        return;
    }
    
    // set callbacks
    glfwSetKeyCallback(glfwWindow, keyCallback);
    glfwSetCharCallback(glfwWindow, charCallback);
    glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
    glfwSetCursorPosCallback(glfwWindow, cursorPosCallback);
    glfwSetCursorEnterCallback(glfwWindow, cursorEnterCallback);
    glfwSetScrollCallback(glfwWindow, scrollCallback);
}

//------------------------------------------------------------------------------
void
glfwInputMgr::discardGlfwCallbacks() {
    GLFWwindow* glfwWindow = Render::glfwDisplayMgr::getGlfwWindow();
    o_assert(nullptr != glfwWindow);
    
    glfwSetKeyCallback(glfwWindow, nullptr);
    glfwSetCharCallback(glfwWindow, nullptr);
    glfwSetMouseButtonCallback(glfwWindow, nullptr);
    glfwSetCursorPosCallback(glfwWindow, nullptr);
    glfwSetCursorEnterCallback(glfwWindow, nullptr);
    glfwSetScrollCallback(glfwWindow, nullptr);
}

} // namespace Input
} // namespace Oryol