//------------------------------------------------------------------------------
//  eglDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "eglDisplayMgr.h"
#if ORYOL_ANDROID
#include "android/native_window.h"
#include "android_native/android_native_app_glue.h"
#endif
#include "Gfx/gl/gl_impl.h"
#include "Gfx/gl/glInfo.h"
#include "Gfx/gl/glExt.h"

#if ORYOL_ANDROID
// this is in the app's main file (see App.h -> OryolApp)
extern android_app* OryolAndroidAppState;
#endif

namespace Oryol {
namespace _priv {

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
eglDisplayMgr::SetupDisplay(const GfxSetup& gfxSetup, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());
    o_assert(nullptr == this->eglDisplay);

    Log::Info("eglDisplayMgr::SetupDisplay() called!\n");

    displayMgrBase::SetupDisplay(gfxSetup, ptrs);

    this->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    o_assert(nullptr != this->eglDisplay);
    o_assert(eglGetError() == EGL_SUCCESS);
    if (!eglInitialize(this->eglDisplay, NULL, NULL)) {
        o_error("eglDisplayMgr: Couldn't initialize EGL!\n");
        return;
    }

    EGLint eglConfigAttrs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_SAMPLES, gfxSetup.Samples,
        EGL_RED_SIZE, PixelFormat::NumBits(gfxSetup.ColorFormat, PixelChannel::Red),
        EGL_GREEN_SIZE, PixelFormat::NumBits(gfxSetup.ColorFormat, PixelChannel::Green),
        EGL_BLUE_SIZE, PixelFormat::NumBits(gfxSetup.ColorFormat, PixelChannel::Blue),
        EGL_ALPHA_SIZE, PixelFormat::NumBits(gfxSetup.ColorFormat, PixelChannel::Alpha),
        EGL_DEPTH_SIZE, PixelFormat::NumBits(gfxSetup.DepthFormat, PixelChannel::Depth),
        EGL_STENCIL_SIZE, PixelFormat::NumBits(gfxSetup.DepthFormat, PixelChannel::Stencil),
        EGL_NONE
    };
    EGLint numConfigs = 0;
    if (!eglChooseConfig(this->eglDisplay, eglConfigAttrs, &this->eglConfig, 1, &numConfigs)) {
        o_error("eglDisplayMgr: eglChooseConfig failed!\n");
        return;
    }
    o_assert(eglGetError() == EGL_SUCCESS);

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

    this->eglSurface = eglCreateWindowSurface(this->eglDisplay, this->eglConfig, window, NULL);
    o_assert(eglGetError() == EGL_SUCCESS);
    o_assert(nullptr != this->eglSurface);

    EGLint contextAttrs[] = {
        #if ORYOL_OPENGLES3
        EGL_CONTEXT_CLIENT_VERSION, 3,
        #else
        EGL_CONTEXT_CLIENT_VERSION, 2,
        #endif
        EGL_NONE
    };
    this->eglContext = eglCreateContext(this->eglDisplay, this->eglConfig, EGL_NO_CONTEXT, contextAttrs);
    o_assert(eglGetError() == EGL_SUCCESS);
    o_assert(nullptr != this->eglContext);

    if (!eglMakeCurrent(this->eglDisplay, this->eglSurface, this->eglSurface, this->eglContext)) {
        o_error("eglDisplayMgr: eglMakeCurrent failed!\n");
        return;
    }

    // setup platform constants and extensions
    glInfo::Setup();
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
    glInfo::Discard();

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

} // namespace _priv
} // namespace Oryol
