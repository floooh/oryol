#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glfwDisplayMgr
    @brief private: display manager class on top of GLFW
    
    This is the display manager class for desktop platforms (OSX, Windows,
    Linux). It uses GLFW ( https://github.com/glfw/glfw ) for window and
    GL context management, and consuming window input events.
*/
#include "Render/base/displayMgrBase.h"
#include "Render/gl/gl_decl.h"

struct GLFWwindow;

namespace Oryol {
namespace Render {
    
class glfwDisplayMgr : public displayMgrBase {
public:
    /// constructor
    glfwDisplayMgr();
    /// destructor
    ~glfwDisplayMgr();
    
    /// setup the display system, must happen before rendering
    void SetupDisplay(const RenderSetup& renderSetup);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// process window system events (call near start of frame)
    void ProcessSystemEvents();
    /// present the current rendered frame
    void Present();
    /// check whether the window system requests to quit the application
    bool QuitRequested() const;
    
    /// bind the default frame buffer
    void glBindDefaultFramebuffer();
    
private:
    /// error callback for GLFW
    static void glfwErrorCallback(int error, const char* desc);
    /// framebuffer size changed callback for GLFW
    static void glwfFramebufferSizeChanged(GLFWwindow* win, int width, int height);

    static glfwDisplayMgr* self;
    GLFWwindow* glfwWindow;
};
    
} // namespace Render
} // namespace Oryol