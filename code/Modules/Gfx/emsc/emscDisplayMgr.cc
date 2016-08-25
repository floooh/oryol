//------------------------------------------------------------------------------
//  emscDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "emscDisplayMgr.h"
#include "Gfx/gl/gl_impl.h"
#include "Gfx/gl/glCaps.h"
#include <emscripten/emscripten.h>

namespace Oryol {
namespace _priv {

emscDisplayMgr* emscDisplayMgr::self = nullptr;

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

    emscripten_set_canvas_size(renderSetup.Width, renderSetup.Height);

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
    Log::Info("emscDisplayMgr: trying to create WebGL2 context...\n");
    glCaps::Flavour glFlavour = glCaps::GLES3;
    ctxAttrs.majorVersion = 2;
    ctxAttrs.minorVersion = 0;
    this->ctx = emscripten_webgl_create_context(nullptr, &ctxAttrs);
    if (this->ctx) {
        Log::Info("emscDisplayMgr: using WebGL2 context\n");
    }
    else {
        // WebGL2 context creation failed, try WebGL1
        Log::Info("emscDisplayMgr: WebGL2 context creation failed, trying WebGL...\n");
        ctxAttrs.majorVersion = 1;
        this->ctx = emscripten_webgl_create_context(nullptr, &ctxAttrs);
        glFlavour = glCaps::GLES2;
    }
    o_assert2(this->ctx > 0, "Failed to create WebGL context");
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
    int newWidth, newHeight, isFullscreen;
    emscripten_get_canvas_size(&newWidth, &newHeight, &isFullscreen);
    self->displayAttrs.FramebufferWidth = newWidth; 
    self->displayAttrs.FramebufferHeight = newHeight;
    return true;
}

//------------------------------------------------------------------------------
extern "C" {

// this function can be called from Javascript to enter 'real' fullscreen mode
void enter_fullscreen() {
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
// NOTE: there's currently no programmatical way to leave this 
// soft fullscreen mode in Oryol (would need a separate Gfx call)
void enter_soft_fullscreen() {
    EmscriptenFullscreenStrategy fsStrategy = { };
    fsStrategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT;
    fsStrategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
    fsStrategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST;
    fsStrategy.canvasResizedCallback = emscDisplayMgr::emscCanvasSizeChanged;
    fsStrategy.canvasResizedCallbackUserData = nullptr;    
    EMSCRIPTEN_RESULT res;
    res = emscripten_enter_soft_fullscreen(nullptr, &fsStrategy);
}

} // extern "C"
} // namespace _priv
} // namespace Oryol

