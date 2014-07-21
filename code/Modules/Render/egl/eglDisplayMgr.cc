//------------------------------------------------------------------------------
//  eglDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "eglDisplayMgr.h"
#include "Render/RenderProtocol.h"
#if ORYOL_EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif
#if ORYOL_ANDROID
#include "android/native_window.h"
#include "android_native/android_native_app_glue.h"
#endif
#include "Render/gl/gl_impl.h"
#include "Render/gl/glInfo.h"
#include "Render/gl/glExt.h"

#if ORYOL_ANDROID
// this is in the app's main file (see App.h -> OryolApp)
extern android_app* OryolAndroidAppState;
#endif

namespace Oryol {
namespace Render {

using namespace Core;
    
//------------------------------------------------------------------------------
eglDisplayMgr::eglDisplayMgr() :
#if ORYOL_EMSCRIPTEN
storedCanvasWidth(0),
storedCanvasHeight(0),
#endif
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
        emscripten_set_canvas_size(renderSetup.WindowWidth, renderSetup.WindowHeight);
    #elif ORYOL_ANDROID
        // nothing to do here
    #else
    #error "Unsupported platform in eglDisplayMgr::SetupDisplay()"
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
    PixelFormat::Code colorPixelFormat = renderSetup.ColorPixelFormat;
    PixelFormat::Code depthPixelFormat = renderSetup.DepthPixelFormat;
    EGLint eglConfigAttrs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
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

    // the window handle
    #if ORYOL_ANDROID
        o_assert(OryolAndroidAppState);
        EGLNativeWindowType window = OryolAndroidAppState->window;
    #else
        EGLNativeWindowType window = 0;
    #endif

    #if ORYOL_ANDROID
    // from the native_activity NDK sample:
    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    EGLint format;
    eglGetConfigAttrib(this->eglDisplay, this->eglConfig, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);
    #endif

    // create window surface
    this->eglSurface = eglCreateWindowSurface(this->eglDisplay, this->eglConfig, window, NULL);
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

    // query actual display size and set in DisplayAttrs
    EGLint actualWidth = 0, actualHeight = 0;
    eglQuerySurface(this->eglDisplay, this->eglSurface, EGL_WIDTH, &actualWidth);
    eglQuerySurface(this->eglDisplay, this->eglSurface, EGL_HEIGHT, &actualHeight);
    Log::Info("eglDisplayMgr: actual framebuffer size w=%d, h=%d\n", actualWidth, actualHeight);
    this->displayAttrs.FramebufferWidth = actualWidth;
    this->displayAttrs.FramebufferHeight = actualHeight;
    this->displayAttrs.WindowWidth = actualWidth;
    this->displayAttrs.WindowHeight = actualHeight;

    // register display-changed callback for emscripten    
    #if ORYOL_EMSCRIPTEN
    EMSCRIPTEN_RESULT res = emscripten_set_fullscreenchange_callback(0, this, 1, emscFullscreenChanged);
    o_assert(EMSCRIPTEN_RESULT_SUCCESS == res);
    #endif
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
    glExt::Discard();

    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
eglDisplayMgr::Present() {
    o_assert(this->eglDisplay && this->eglSurface);
    eglSwapBuffers(this->eglDisplay, this->eglSurface);
    displayMgrBase::Present();
}

//------------------------------------------------------------------------------
void
eglDisplayMgr::glBindDefaultFramebuffer() {
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
#if ORYOL_EMSCRIPTEN
EM_BOOL
eglDisplayMgr::emscFullscreenChanged(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData) {
    eglDisplayMgr* self = (eglDisplayMgr*) userData;
    o_assert(nullptr != self);

    // NOTE: there's currently a bug that the reported element size is wrong, so use 
    // the fullscreen size instead
    Log::Info("emscFullscreenChanged: isFullscreen=%s, fullscreenEnabled=%s, ew=%d, eh=%d, sw=%d, sh=%d\n",
        e->isFullscreen ? "yes":"no", 
        e->fullscreenEnabled ? "yes":"no", 
        e->elementWidth, 
        e->elementHeight,
        e->screenWidth,
        e->screenHeight);

    int newWidth, newHeight;
    if (e->isFullscreen) {
        // switch to fullscreen
        newWidth = e->screenWidth;
        newHeight = e->screenHeight;
        self->storedCanvasWidth = self->displayAttrs.FramebufferWidth;
        self->storedCanvasHeight = self->displayAttrs.FramebufferHeight;
    }
    else {
        // switch back from fullscreen
        newWidth = self->storedCanvasWidth;
        newHeight = self->storedCanvasHeight;
    }
    emscripten_set_canvas_size(newWidth, newHeight);
    self->displayAttrs.FramebufferWidth = newWidth;
    self->displayAttrs.FramebufferHeight = newHeight;
    Log::Info("emscFullscreenChanged: new canvas size (w=%d, h=%d)\n", newWidth, newHeight);

    // notify event handlers
    self->notifyEventHandlers(RenderProtocol::DisplayModified::Create());

    return true;
}
#endif

} // namespace Render
} // namespace Oryol
