#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glfwDisplayMgr
    @ingroup _priv
    @brief display manager class on top of GLFW
    
    This is the display manager class for desktop platforms (OSX, Windows,
    Linux). It uses GLFW ( https://github.com/glfw/glfw ) for window and
    GL context management, and consuming window input events.
*/
#include "Gfx/private/displayMgrBase.h"
#include "Gfx/private/gl/gl_decl.h"

struct GLFWwindow;

namespace Oryol {
namespace _priv {
    
class glfwDisplayMgr : public displayMgrBase {
public:
    /// constructor
    glfwDisplayMgr();
    /// destructor
    ~glfwDisplayMgr();
    
    /// setup the display system, must happen before rendering
    void SetupDisplay(const GfxSetup& gfxSetup, const gfxPointers& ptrs);
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
    
    /// get glfwWindow handle
    static GLFWwindow* getGlfwWindow();
    
    /// error callback for GLFW
    static void glfwErrorCallback(int error, const char* desc);
    /// framebuffer size changed callback for GLFW
    static void glwfFramebufferSizeChanged(GLFWwindow* win, int width, int height);
    /// create the main window
    void createMainWindow(const GfxSetup& setup);
    /// destroy the main window
    void destroyMainWindow();

    static glfwDisplayMgr* self;
    static GLFWwindow* glfwWindow;
};
    
} // namespace _priv
} // namespace Oryol
