//------------------------------------------------------------------------------
//  eglDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "eglDisplayMgr.h"
#if ORYOL_EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif
#include "Render/gl/gl_impl.h"
#include "Render/gl/glInfo.h"
#include "Render/gl/glExt.h"

namespace Oryol {
namespace Render {

using namespace Core;
    
//------------------------------------------------------------------------------
eglDisplayMgr::eglDisplayMgr() :
eglDisplay(nullptr),
eglConfig(nullptr),
eglSurface(nullptr),
eglContext(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
eglDisplayMgr::~eglDisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
}

//------------------------------------------------------------------------------
void
eglDisplayMgr::SetupDisplay(const RenderSetup& renderSetup) {
    o_assert(!this->IsDisplayValid());
    o_assert(nullptr == this->eglDisplay);

    Log::Info("eglDisplayMgr::SetupDisplay() called!\n");

    displayMgrBase::SetupDisplay(renderSetup);

    // this is the window-system specific part
    #if ORYOL_EMSCRIPTEN
    emscripten_set_canvas_size(renderSetup.GetWindowWidth(), renderSetup.GetWindowHeight());
    #else
    #error "Unsupported platform in eglDisplay::SetupDisplay()"
    #endif

    // EGL stuff starts here
    this->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    o_assert(nullptr != this->eglDisplay);
    o_assert(eglGetError() == EGL_SUCCESS);
    if (!eglInitialize(this->eglDisplay, NULL, NULL)) {
        o_error("eglDisplayMgr: Couldn't initialize EGL!\n");
        return;
    }

    // choose EGL config
    PixelFormat::Code colorPixelFormat = renderSetup.GetColorPixelFormat();
    PixelFormat::Code depthPixelFormat = renderSetup.GetDepthPixelFormat();
    EGLint eglConfigAttrs[] = {
        EGL_RED_SIZE, PixelFormat::NumBits(colorPixelFormat, PixelFormat::Red),
        EGL_GREEN_SIZE, PixelFormat::NumBits(colorPixelFormat, PixelFormat::Green),
        EGL_BLUE_SIZE, PixelFormat::NumBits(colorPixelFormat, PixelFormat::Blue),
        EGL_ALPHA_SIZE, PixelFormat::NumBits(colorPixelFormat, PixelFormat::Alpha),
        EGL_DEPTH_SIZE, PixelFormat::NumBits(depthPixelFormat, PixelFormat::Depth),
        EGL_STENCIL_SIZE, PixelFormat::NumBits(depthPixelFormat, PixelFormat::Stencil),
        EGL_NONE
    };
    EGLint numConfigs = 0;
    if (!eglChooseConfig(this->eglDisplay, eglConfigAttrs, &this->eglConfig, 1, &numConfigs)) {
        o_error("eglDisplayMgr: eglChooseConfig failed!\n");
        return;
    }
    o_assert(eglGetError() == EGL_SUCCESS);

    // create window surface
    EGLNativeWindowType dummyWindow = 0;
    this->eglSurface = eglCreateWindowSurface(this->eglDisplay, this->eglConfig, dummyWindow, NULL);
    o_assert(eglGetError() == EGL_SUCCESS);
    o_assert(nullptr != this->eglSurface);

    // and finally create the context
    EGLint contextAttrs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    this->eglContext = eglCreateContext(this->eglDisplay, this->eglConfig, EGL_NO_CONTEXT, contextAttrs);
    o_assert(eglGetError() == EGL_SUCCESS);
    o_assert(nullptr != this->eglContext);

    // make the context current
    if (!eglMakeCurrent(this->eglDisplay, this->eglSurface, this->eglSurface, this->eglContext)) {
        o_error("eglDisplayMgr: eglMakeCurrent failed!\n");
        return;
    }

    // dump GL information
    glInfo::PrintInfo();

    // setup extensions
    glExt::Setup();
}

//------------------------------------------------------------------------------
void
eglDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());

    eglDestroyContext(this->eglDisplay, this->eglContext);
    this->eglContext = nullptr;
    eglDestroySurface(this->eglDisplay, this->eglSurface);
    this->eglSurface = nullptr;
    eglTerminate(this->eglDisplay);
    this->eglConfig = nullptr;
    this->eglDisplay = nullptr;

    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
GLuint
eglDisplayMgr::glGetDefaultFramebuffer() const {
    return 0;
}

} // namespace Render
} // namespace Oryol
