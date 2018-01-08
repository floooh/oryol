//------------------------------------------------------------------------------
//  eglDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "eglDisplayMgr.h"
#if ORYOL_ANDROID
#include "android/native_window.h"
#include "Core/private/android/android_native_app_glue.h"
#endif
#if ORYOL_RASPBERRYPI
#include "bcm_host.h"
#endif
#include "Gfx/private/gl/gl_impl.h"
#include "Gfx/private/gl/glCaps.h"

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

    #if ORYOL_RASPBERRYPI
    bcm_host_init();
    #endif

    this->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    o_assert(nullptr != this->eglDisplay);
    o_assert(eglGetError() == EGL_SUCCESS);
    if (!eglInitialize(this->eglDisplay, NULL, NULL)) {
        o_error("eglDisplayMgr: Couldn't initialize EGL!\n");
        return;
    }

    // make sure we have a valid rendering RGBA format, e.g. RGB is
    // not a valid framebuffer format
    int colorBits = PixelFormat::NumBits(gfxSetup.ColorFormat, PixelChannel::Red);
    o_assert((colorBits == 4) || (colorBits == 8));
    EGLint eglConfigAttrs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_SAMPLES, gfxSetup.SampleCount,
        EGL_RED_SIZE, colorBits,
        EGL_GREEN_SIZE, colorBits,
        EGL_BLUE_SIZE, colorBits,
        EGL_ALPHA_SIZE, colorBits,
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
   
    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        o_error("eglDisplayMgr: eglBindAPI() failed!\n");
        return;
    }

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

    #if ORYOL_ANDROID
        o_assert(OryolAndroidAppState);
        EGLNativeWindowType window = OryolAndroidAppState->window;
        // from the native_activity NDK sample:
        /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
         * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
         * As soon as we picked a EGLConfig, we can safely reconfigure the
         * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
        EGLint format;
        eglGetConfigAttrib(this->eglDisplay, this->eglConfig, EGL_NATIVE_VISUAL_ID, &format);
        int32_t w = ANativeWindow_getWidth(window);
        int32_t h = ANativeWindow_getHeight(window);
        if (!gfxSetup.HighDPI) {
            w/=2; h/=2;
        }
        ANativeWindow_setBuffersGeometry(window, w, h, format);
    #elif ORYOL_RASPBERRYPI
        static EGL_DISPMANX_WINDOW_T rpiNativeWindow;
        uint32_t screenWidth, screenHeight;
        int32_t success = graphics_get_display_size(0, &screenWidth, &screenHeight);
        o_assert(success >= 0);
        
        VC_RECT_T dstRect, srcRect;
        dstRect.x = dstRect.y = 0;
        dstRect.width = screenWidth;
        dstRect.height = screenHeight;
        srcRect.x = srcRect.y = 0;
        srcRect.width = screenWidth << 16;
        srcRect.height = screenHeight << 16;
         
        DISPMANX_DISPLAY_HANDLE_T dispmanDisplay = vc_dispmanx_display_open(0);
        DISPMANX_UPDATE_HANDLE_T dispmanUpdate = vc_dispmanx_update_start(0);
        DISPMANX_ELEMENT_HANDLE_T dispmanElement;
        dispmanElement = vc_dispmanx_element_add(
            dispmanUpdate, 
            dispmanDisplay,
            0, // layer
            &dstRect, 
            0, // src
            &srcRect,
            DISPMANX_PROTECTION_NONE,
            0,  // alpha
            0,  // clamp
            DISPMANX_NO_ROTATE); // transform

        rpiNativeWindow.element = dispmanElement;
        rpiNativeWindow.width = screenWidth;
        rpiNativeWindow.height = screenHeight;
        vc_dispmanx_update_submit_sync(dispmanUpdate);

        EGLNativeWindowType window = &rpiNativeWindow;
    #else
        EGLNativeWindowType window = 0;
    #endif

    this->eglSurface = eglCreateWindowSurface(this->eglDisplay, this->eglConfig, window, NULL);
    o_assert(eglGetError() == EGL_SUCCESS);
    o_assert(nullptr != this->eglSurface);
    if (!eglMakeCurrent(this->eglDisplay, this->eglSurface, this->eglSurface, this->eglContext)) {
        o_error("eglDisplayMgr: eglMakeCurrent failed!\n");
        return;
    }
    #if ORYOL_OPENGLES3
        glCaps::Setup(glCaps::GLES3);
    #else
        glCaps::Setup(glCaps::GLES2);
    #endif
    
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
    glCaps::Discard();

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

