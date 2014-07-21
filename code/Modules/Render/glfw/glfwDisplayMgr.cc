//------------------------------------------------------------------------------
//  glfwDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/gl/gl_impl.h"
#include "Render/gl/glInfo.h"
#include "Render/gl/glExt.h"
#include "Render/RenderProtocol.h"
#include "glfwDisplayMgr.h"
#include "Core/Log.h"
#if ORYOL_MACOS
#define GLFW_INCLUDE_GLCOREARB
#endif
#include "GLFW/glfw3.h"

namespace Oryol {
namespace Render {
    
glfwDisplayMgr* glfwDisplayMgr::self = nullptr;
    
//------------------------------------------------------------------------------
glfwDisplayMgr::glfwDisplayMgr() :
glfwWindow(nullptr) {
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
glfwDisplayMgr::~glfwDisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
    o_assert(nullptr != self);
    self = nullptr;
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::SetupDisplay(const RenderSetup& setup) {
    o_assert(!this->IsDisplayValid());
    o_assert(nullptr == this->glfwWindow);
    
    displayMgrBase::SetupDisplay(setup);
    
    // setup GLFW
    if (!glfwInit()) {
        o_error("glfwDisplayMgr: failed to setup GLFW!\n");
    }
    glfwSetErrorCallback(glfwErrorCallback);
    
    // setup the GLFW window
    PixelFormat::Code colorPixelFormat = setup.ColorPixelFormat;
    PixelFormat::Code depthPixelFormat = setup.DepthPixelFormat;
    glfwWindowHint(GLFW_RED_BITS, PixelFormat::NumBits(colorPixelFormat, PixelFormat::Red));
    glfwWindowHint(GLFW_GREEN_BITS, PixelFormat::NumBits(colorPixelFormat, PixelFormat::Green));
    glfwWindowHint(GLFW_BLUE_BITS, PixelFormat::NumBits(colorPixelFormat, PixelFormat::Blue));
    glfwWindowHint(GLFW_ALPHA_BITS, PixelFormat::NumBits(colorPixelFormat, PixelFormat::Alpha));
    glfwWindowHint(GLFW_DEPTH_BITS, PixelFormat::NumBits(depthPixelFormat, PixelFormat::Depth));
    glfwWindowHint(GLFW_STENCIL_BITS, PixelFormat::NumBits(colorPixelFormat, PixelFormat::Stencil));
    #if ORYOL_MACOS
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    // windowed or fullscreen mode?
    GLFWmonitor* glfwMonitor = nullptr;
    if (setup.IsFullscreen) {
        glfwMonitor = glfwGetPrimaryMonitor();
    }
    
    // now actually create the window
    this->glfwWindow = glfwCreateWindow(setup.WindowWidth,
                                        setup.WindowHeight,
                                        setup.WindowTitle.AsCStr(),
                                        glfwMonitor,
                                        0);
    o_assert(nullptr != this->glfwWindow);
    
    // and make the window's GL context current
    glfwMakeContextCurrent(this->glfwWindow);
    glfwSwapInterval(setup.SwapInterval);

    // dump GL information
    glInfo::PrintInfo();

    // setup extensions
    glExt::Setup();
    
    // now set the actual display attributes
    int fbWidth = 0, fbHeight = 0;
    int posX = 0, posY = 0;
    int width = 0, height = 0;
    glfwGetFramebufferSize(this->glfwWindow, &fbWidth, &fbHeight);
    glfwGetWindowPos(this->glfwWindow, &posX, &posY);
    glfwGetWindowSize(this->glfwWindow, &width, &height);
    this->displayAttrs.FramebufferWidth  = fbWidth;
    this->displayAttrs.FramebufferHeight = fbHeight;
    this->displayAttrs.WindowPosX        = posX;
    this->displayAttrs.WindowPosY        = posY;
    this->displayAttrs.WindowWidth       = width;
    this->displayAttrs.WindowHeight      = height;
    
    // set framebuffer size changed callback
    glfwSetFramebufferSizeCallback(this->glfwWindow, glwfFramebufferSizeChanged);
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());
    o_assert(nullptr != this->glfwWindow);
    
    glfwDestroyWindow(this->glfwWindow);
    this->glfwWindow = nullptr;
    glfwTerminate();
    glExt::Discard();
    
    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
bool
glfwDisplayMgr::QuitRequested() const {
    o_assert(nullptr != this->glfwWindow);
    return glfwWindowShouldClose(this->glfwWindow);
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::ProcessSystemEvents() {
    o_assert(nullptr != this->glfwWindow);
    glfwPollEvents();
    displayMgrBase::ProcessSystemEvents();
}


//------------------------------------------------------------------------------
void
glfwDisplayMgr::Present() {
    o_assert(nullptr != this->glfwWindow);
    glfwSwapBuffers(this->glfwWindow);
    displayMgrBase::Present();
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::glBindDefaultFramebuffer() {
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::glfwErrorCallback(int error, const char* desc) {
    Core::Log::Error("GLFW error: '%d', '%s'\n", error, desc);
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::glwfFramebufferSizeChanged(GLFWwindow* win, int width, int height) {

    // update display attributes
    self->displayAttrs.FramebufferWidth = width;
    self->displayAttrs.FramebufferHeight = height;
    int winWidth, winHeight;
    glfwGetWindowSize(self->glfwWindow, &winWidth, &winHeight);
    self->displayAttrs.WindowWidth = winWidth;
    self->displayAttrs.WindowHeight = winHeight;
    
    // notify event handlers
    self->notifyEventHandlers(RenderProtocol::DisplayModified::Create());
}

} // namespace Render
} // namespace Oryol
