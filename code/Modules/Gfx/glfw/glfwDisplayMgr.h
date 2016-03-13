#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glfwDisplayMgr
    @ingroup _priv
    @brief private: display manager class on top of GLFW
    
    This is the display manager class for desktop platforms (OSX, Windows,
    Linux). It uses GLFW ( https://github.com/glfw/glfw ) for window and
    GL/Vulkan context management, and consuming window input events.
*/
#include "Gfx/Core/displayMgrBase.h"
#if ORYOL_VULKAN
#include "Gfx/vlk/vlkContext.h"
#else
#include "Gfx/gl/gl_decl.h"
#endif

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
    
    #if ORYOL_OPENGL
    /// bind the default frame buffer
    void glBindDefaultFramebuffer();
    #endif

    /// private: get glfwWindow handle
    static GLFWwindow* getGlfwWindow();
    
private:
    #if ORYOL_VULKAN
    /// vulkan-specific init
    void setupVulkan(const GfxSetup& setup);
    /// vulkan-specific teardown
    void discardVulkan();
    #else
    /// GL-specific init
    void setupGL(const GfxSetup& setup);
    /// GL-specific teardown
    void discardGL();
    #endif
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

    #if ORYOL_VULKAN
    class vlkContext vlkContext;
    #endif
};
    
} // namespace _priv
} // namespace Oryol
