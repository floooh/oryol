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

class GLFWWindow;

namespace Oryol {
namespace Render {
    
class glfwDisplayMgr : public displayMgrBase {
public:
    /// constructor
    glfwDisplayMgr();
    /// destructor
    ~glfwDisplayMgr();
    
    /// setup the display system, must happen before rendering
    void SetupDisplay(const DisplaySetup& displaySetup);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// process window system events (call near start of frame)
    void ProcessSystemEvents();
    /// present the current rendered frame
    void Present();
    
private:
    GLFWWindow* glfwWindow;
};
    
} // namespace Render
} // namespace Oryol