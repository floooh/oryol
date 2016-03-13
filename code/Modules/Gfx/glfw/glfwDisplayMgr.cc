//------------------------------------------------------------------------------
//  glfwDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#if ORYOL_OPENGL
#include "Gfx/gl/gl_impl.h"
#include "Gfx/gl/glInfo.h"
#include "Gfx/gl/glExt.h"
#include "Gfx/gl/glDebugOutput.h"
#endif
#include "glfwDisplayMgr.h"
#include "Core/Log.h"
#include "Core/String/StringBuilder.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace Oryol {
namespace _priv {
    
glfwDisplayMgr* glfwDisplayMgr::self = nullptr;
GLFWwindow* glfwDisplayMgr::glfwWindow = nullptr;
    
//------------------------------------------------------------------------------
glfwDisplayMgr::glfwDisplayMgr() {
    o_assert(nullptr == self);
    o_assert(nullptr == glfwWindow);
    self = this;
}

//------------------------------------------------------------------------------
glfwDisplayMgr::~glfwDisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
    o_assert(nullptr == glfwWindow);
    o_assert(nullptr != self);
    self = nullptr;
}

//------------------------------------------------------------------------------
GLFWwindow*
glfwDisplayMgr::getGlfwWindow() {
    return glfwWindow;
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());
    
    displayMgrBase::SetupDisplay(setup, ptrs);
    
    // setup GLFW
    if (!glfwInit()) {
        o_error("glfwDisplayMgr: failed to setup GLFW!\n");
    }
    glfwSetErrorCallback(glfwErrorCallback);
    
    #if ORYOL_VULKAN
    this->setupVulkan(setup);
    #else
    this->setupGL(setup);
    #endif

    // now set the actual display attributes
    int fbWidth = 0, fbHeight = 0;
    int posX = 0, posY = 0;
    int width = 0, height = 0;
    glfwGetFramebufferSize(glfwWindow, &fbWidth, &fbHeight);
    glfwGetWindowPos(glfwWindow, &posX, &posY);
    glfwGetWindowSize(glfwWindow, &width, &height);
    this->displayAttrs.FramebufferWidth  = fbWidth;
    this->displayAttrs.FramebufferHeight = fbHeight;
    this->displayAttrs.WindowPosX        = posX;
    this->displayAttrs.WindowPosY        = posY;
    this->displayAttrs.WindowWidth       = width;
    this->displayAttrs.WindowHeight      = height;
    
    // set framebuffer size changed callback
    glfwSetFramebufferSizeCallback(glfwWindow, glwfFramebufferSizeChanged);
}

//------------------------------------------------------------------------------
#if ORYOL_OPENGL
void
glfwDisplayMgr::setupGL(const GfxSetup& setup) {

    // GL-specific window hints
    #if ORYOL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    #endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // setup the GLFW main window
    this->createMainWindow(setup);

    // and make the window's GL context current
    glfwMakeContextCurrent(this->glfwWindow);
    glfwSwapInterval(setup.SwapInterval);

    // setup GL extensions and platform-dependent constants
    ORYOL_GL_CHECK_ERROR();
    flextInit(glfwWindow);
    ORYOL_GL_CHECK_ERROR();
    glInfo::Setup();
    glExt::Setup();
    #if ORYOL_DEBUG
    glDebugOutput::Enable(glDebugOutput::Medium);
    #endif
}
#endif

//------------------------------------------------------------------------------
#if ORYOL_VULKAN
void
glfwDisplayMgr::setupVulkan(const GfxSetup& setup) {

    if (!glfwVulkanSupported()) {
        // FIXME: need better error output (MessageBox)
        o_error("Failed to find Vulkan loader\n");
    }
    // initialize Vulkan instance and device
    uint32_t numRequiredExtensions = 0;
    const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&numRequiredExtensions);
    this->vlkContext.setup(setup, requiredExtensions, numRequiredExtensions);

    // Vulkan-specific window hints
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        
    // create the application window
    this->createMainWindow(setup);

    // FIXME: initialize Vulkan swap chain
}
#endif

//------------------------------------------------------------------------------
void
glfwDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());
    o_assert(nullptr != glfwWindow);

    #if ORYOL_VULKAN
    this->discardVulkan();
    #else
    this->discardGL();
    #endif    
    glfwTerminate();
    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
#if ORYOL_VULKAN
void
glfwDisplayMgr::discardVulkan() {
    // FIXME: destroy swapchain
    this->destroyMainWindow();
    this->vlkContext.discard();
}
#endif

//------------------------------------------------------------------------------
#if ORYOL_OPENGL
void
glfwDisplayMgr::discardGL() {
    this->destroyMainWindow();
    glExt::Discard();
    glInfo::Discard();
}
#endif

//------------------------------------------------------------------------------
bool
glfwDisplayMgr::QuitRequested() const {
    o_assert(nullptr != glfwWindow);
    
    return glfwWindowShouldClose(glfwWindow) != 0;
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::ProcessSystemEvents() {
    o_assert(nullptr != glfwWindow);
    
    glfwPollEvents();
    displayMgrBase::ProcessSystemEvents();
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::Present() {
    o_assert(nullptr != glfwWindow);

    #if ORYOL_OPENGL
    glfwSwapBuffers(glfwWindow);
    #endif

    displayMgrBase::Present();
}

//------------------------------------------------------------------------------
#if ORYOL_OPENGL
void
glfwDisplayMgr::glBindDefaultFramebuffer() {
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ORYOL_GL_CHECK_ERROR();
}
#endif

//------------------------------------------------------------------------------
void
glfwDisplayMgr::glfwErrorCallback(int error, const char* desc) {
    Log::Error("GLFW error: '%d', '%s'\n", error, desc);
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::glwfFramebufferSizeChanged(GLFWwindow* win, int width, int height) {

    // update display attributes (ignore window-minimized)
    if ((width != 0) && (height != 0)) {
        self->displayAttrs.FramebufferWidth = width;
        self->displayAttrs.FramebufferHeight = height;
        int winWidth, winHeight;
        glfwGetWindowSize(glfwWindow, &winWidth, &winHeight);
        self->displayAttrs.WindowWidth = winWidth;
        self->displayAttrs.WindowHeight = winHeight;
    }
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::createMainWindow(const GfxSetup& setup) {
    o_assert_dbg(nullptr == glfwDisplayMgr::glfwWindow);

    #if ORYOL_MACOS
    // work around a bug on OSX where a 16-bit color buffer is created when alpha-bits are set 0
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8); 
    glfwWindowHint(GLFW_BLUE_BITS, 8); 
    glfwWindowHint(GLFW_ALPHA_BITS, 8); 
    #else
    glfwWindowHint(GLFW_RED_BITS, PixelFormat::NumBits(setup.ColorFormat, PixelChannel::Red));
    glfwWindowHint(GLFW_GREEN_BITS, PixelFormat::NumBits(setup.ColorFormat, PixelChannel::Green));
    glfwWindowHint(GLFW_BLUE_BITS, PixelFormat::NumBits(setup.ColorFormat, PixelChannel::Blue));
    glfwWindowHint(GLFW_ALPHA_BITS, PixelFormat::NumBits(setup.ColorFormat, PixelChannel::Alpha));
    #endif
    glfwWindowHint(GLFW_DEPTH_BITS, PixelFormat::NumBits(setup.DepthFormat, PixelChannel::Depth));
    glfwWindowHint(GLFW_STENCIL_BITS, PixelFormat::NumBits(setup.DepthFormat, PixelChannel::Stencil));
    glfwWindowHint(GLFW_SAMPLES, setup.SampleCount > 1 ? setup.SampleCount : 0);

    // windowed or fullscreen mode?
    GLFWmonitor* glfwMonitor = nullptr;
    if (!setup.Windowed) {
        glfwMonitor = glfwGetPrimaryMonitor();
    }
    
    // now actually create the window
    StringBuilder strBuilder(setup.Title);
    #if ORYOL_VULKAN
    strBuilder.Append(" (Vulkan)");
#else
    strBuilder.Append(" (GL)");
    #endif   
    glfwDisplayMgr::glfwWindow = glfwCreateWindow(setup.Width, setup.Height, strBuilder.AsCStr(), glfwMonitor, 0);
    o_assert(nullptr != glfwDisplayMgr::glfwWindow);
}

//------------------------------------------------------------------------------
void
glfwDisplayMgr::destroyMainWindow() {
    o_assert_dbg(nullptr != glfwDisplayMgr::glfwWindow);
    glfwDestroyWindow(glfwWindow);
    glfwWindow = nullptr;
}

} // namespace _priv
} // namespace Oryol
