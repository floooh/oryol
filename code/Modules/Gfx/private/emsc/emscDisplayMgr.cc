//------------------------------------------------------------------------------
//  emscDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "emscDisplayMgr.h"
#include "Gfx/private/gl/gl_impl.h"
#include "Gfx/private/gl/glCaps.h"
#include <emscripten/emscripten.h>

namespace Oryol {

namespace _priv {

emscDisplayMgr* emscDisplayMgr::self = nullptr;

//------------------------------------------------------------------------------
extern "C" {

// this function can be called from Javascript to enter 'real' fullscreen mode
EMSCRIPTEN_KEEPALIVE void enter_fullscreen() {
    EmscriptenFullscreenStrategy fsStrategy = { };
    fsStrategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT;
    fsStrategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
    fsStrategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST;
    fsStrategy.canvasResizedCallback = emscDisplayMgr::emscCanvasSizeChanged;
    fsStrategy.canvasResizedCallbackUserData = nullptr;    
    EMSCRIPTEN_RESULT res;
    res = emscripten_request_fullscreen_strategy(nullptr, false, &fsStrategy);
}

// this function can be called from Javascript to enter 'soft' fullscreen mode
static bool soft_fullscreen_active = false;

EMSCRIPTEN_KEEPALIVE void enter_soft_fullscreen() {
    EmscriptenFullscreenStrategy fsStrategy = { };
    fsStrategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT;
    fsStrategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
    fsStrategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST;
    fsStrategy.canvasResizedCallback = emscDisplayMgr::emscCanvasSizeChanged;
    fsStrategy.canvasResizedCallbackUserData = nullptr;    
    EMSCRIPTEN_RESULT res;
    res = emscripten_enter_soft_fullscreen(nullptr, &fsStrategy);
    soft_fullscreen_active = true;
    Log::Info("enter_soft_fullscreen called!\n");
}

EMSCRIPTEN_KEEPALIVE void leave_soft_fullscreen() {
    emscripten_exit_soft_fullscreen();
    soft_fullscreen_active = false;
    Log::Info("leave_soft_fullscreen called!\n");
}

EMSCRIPTEN_KEEPALIVE bool is_soft_fullscreen_active() {
    return soft_fullscreen_active;
}

} // extern "C"

//------------------------------------------------------------------------------
emscDisplayMgr::emscDisplayMgr() :
storedCanvasWidth(0),
storedCanvasHeight(0),
ctx(0) {
    self = this;
}

//------------------------------------------------------------------------------
emscDisplayMgr::~emscDisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
    self = nullptr;
}

//------------------------------------------------------------------------------
void
emscDisplayMgr::SetupDisplay(const GfxSetup& renderSetup, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());
    displayMgrBase::SetupDisplay(renderSetup, ptrs);

    if (renderSetup.HtmlTrackElementSize) {
        // register notification callback when canvas size changes
        double width, height;
        if (EMSCRIPTEN_RESULT_SUCCESS == emscripten_get_element_css_size(renderSetup.HtmlElement.AsCStr(), &width, &height)) {
            this->displayAttrs.FramebufferWidth = (int) width;
            this->displayAttrs.FramebufferHeight = (int) height;
            Log::Info("Tracked HTML element size '%s': %dx%d\n", 
                renderSetup.HtmlElement.AsCStr(),
                this->displayAttrs.FramebufferWidth,
                this->displayAttrs.FramebufferHeight);
        }
        emscripten_set_canvas_element_size(renderSetup.HtmlElement.AsCStr(), this->displayAttrs.FramebufferWidth, this->displayAttrs.FramebufferHeight);
        emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, false, emscWindowSizeChanged); 
    } else if (renderSetup.Windowed) {
        emscripten_set_canvas_element_size(renderSetup.HtmlElement.AsCStr(), renderSetup.Width, renderSetup.Height);
    }
    else {
        enter_soft_fullscreen();
    }

    EmscriptenWebGLContextAttributes ctxAttrs;
    emscripten_webgl_init_context_attributes(&ctxAttrs);
    ctxAttrs.alpha = 0 < PixelFormat::NumBits(renderSetup.ColorFormat, PixelChannel::Alpha);
    ctxAttrs.depth = 0 < PixelFormat::NumBits(renderSetup.DepthFormat, PixelChannel::Depth);
    ctxAttrs.stencil = 0 < PixelFormat::NumBits(renderSetup.DepthFormat, PixelChannel::Stencil);
    ctxAttrs.antialias = renderSetup.SampleCount > 1;
    ctxAttrs.premultipliedAlpha = false;
    ctxAttrs.preserveDrawingBuffer = false;
    Log::Info("emscDisplayMgr: alpha=%d, depth=%d, stencil=%d, antialias=%d\n", 
        ctxAttrs.alpha, ctxAttrs.depth, ctxAttrs.stencil, ctxAttrs.antialias);
    
    // first try to get an WebGL2 context
    glCaps::Flavour glFlavour = glCaps::GLES3;
    ctxAttrs.minorVersion = 0;
    #if ORYOL_OPENGLES3
    Log::Info("emscDisplayMgr: trying to create WebGL2 context...\n");
    ctxAttrs.majorVersion = 2;
    this->ctx = emscripten_webgl_create_context(renderSetup.HtmlElement.AsCStr(), &ctxAttrs);
    if (this->ctx) {
        Log::Info("emscDisplayMgr: using WebGL2 context\n");
    }
    else
    #endif
    {
        // WebGL2 context creation failed, try WebGL1
        Log::Info("emscDisplayMgr: using WebGL1 context...\n");
        ctxAttrs.majorVersion = 1;
        this->ctx = emscripten_webgl_create_context(renderSetup.HtmlElement.AsCStr(), &ctxAttrs);
        glFlavour = glCaps::GLES2;
    }
    o_assert2(this->ctx > 0, "Failed to create WebGL context");

    // in case we run on an iOS device, need to enabled the 
    // WEBKIT_WEBGL_compressed_texture_pvrtc, this is not done automatically
    // by emscripten (only for WEBGL_compressed_texture_pvrtc)
    if (glFlavour == glCaps::GLES2) {
        Log::Info("emscDisplayMgr: try enabling WEBKIT_WEBGL_compressed_texture_pvrtc\n");
        if (emscripten_webgl_enable_extension(this->ctx, "WEBKIT_WEBGL_compressed_texture_pvrtc")) {
            Log::Info("emscDisplayMgr: WEBKIT_WEBGL_compressed_texture_pvrtc enabled\n");
        }
        else {
            Log::Info("emscDisplayMgr: WEBKIT_WEBGL_compressed_texture_pvrtc not supported\n");
        }
    }
    emscripten_webgl_make_context_current(this->ctx);

    glCaps::Setup(glFlavour);
}

//------------------------------------------------------------------------------
void
emscDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());
    o_assert(0 != this->ctx);

    glCaps::Discard();
    emscripten_webgl_destroy_context(this->ctx);
    this->ctx = 0;

    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
emscDisplayMgr::glBindDefaultFramebuffer() {
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
EM_BOOL
emscDisplayMgr::emscCanvasSizeChanged(int eventType, const void* reserved, void* userData) {
    int newWidth, newHeight;
    emscripten_get_canvas_element_size(self->gfxSetup.HtmlElement.AsCStr(), &newWidth, &newHeight);
    self->displayAttrs.FramebufferWidth = newWidth; 
    self->displayAttrs.FramebufferHeight = newHeight;
    return true;
}

//------------------------------------------------------------------------------
EM_BOOL
emscDisplayMgr::emscWindowSizeChanged(int eventType, const EmscriptenUiEvent* uiEvent, void* userData) {
    double width, height;
    if (EMSCRIPTEN_RESULT_SUCCESS == emscripten_get_element_css_size(self->gfxSetup.HtmlElement.AsCStr(), &width, &height)) {
        self->displayAttrs.FramebufferWidth = (int) width;
        self->displayAttrs.FramebufferHeight = (int) height;
        emscripten_set_canvas_element_size(self->gfxSetup.HtmlElement.AsCStr(), self->displayAttrs.FramebufferWidth, self->displayAttrs.FramebufferHeight);
    }
    return true;
}

} // namespace _priv
} // namespace Oryol

