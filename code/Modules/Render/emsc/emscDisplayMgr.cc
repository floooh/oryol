//------------------------------------------------------------------------------
//  emscDisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "emscDisplayMgr.h"
#include "Render/gl/glInfo.h"
#include "Render/gl/glExt.h"
#include "Render/gl/gl_impl.h"
#include "Render/RenderProtocol.h"
#include <emscripten/emscripten.h>

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
emscDisplayMgr::emscDisplayMgr() :
storedCanvasWidth(0),
storedCanvasHeight(0),
ctx(0) {
    // empty
}

//------------------------------------------------------------------------------
emscDisplayMgr::~emscDisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
}

//------------------------------------------------------------------------------
void
emscDisplayMgr::SetupDisplay(const RenderSetup& renderSetup) {
    o_assert(!this->IsDisplayValid());
    displayMgrBase::SetupDisplay(renderSetup);

    emscripten_set_canvas_size(renderSetup.Width, renderSetup.Height);

    EmscriptenWebGLContextAttributes ctxAttrs;
    emscripten_webgl_init_context_attributes(&ctxAttrs);
    ctxAttrs.alpha = 0 < PixelFormat::NumBits(renderSetup.ColorFormat, Channel::Alpha);
    ctxAttrs.depth = 0 < PixelFormat::NumBits(renderSetup.DepthFormat, Channel::Depth);
    ctxAttrs.stencil = 0 < PixelFormat::NumBits(renderSetup.DepthFormat, Channel::Stencil);
    ctxAttrs.antialias = renderSetup.Samples > 0;
    ctxAttrs.premultipliedAlpha = false;
    this->ctx = emscripten_webgl_create_context(nullptr, &ctxAttrs);
    o_assert2(this->ctx > 0, "Failed to create WebGL context");
    emscripten_webgl_make_context_current(this->ctx);

    EMSCRIPTEN_RESULT res = emscripten_set_fullscreenchange_callback(0, this, 1, emscFullscreenChanged);
    o_assert(EMSCRIPTEN_RESULT_SUCCESS == res);

    glInfo::Setup();
    glExt::Setup();
}

//------------------------------------------------------------------------------
void
emscDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());
    o_assert(0 != this->ctx);

    glExt::Discard();
    glInfo::Discard();
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
emscDisplayMgr::emscFullscreenChanged(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData) {
    emscDisplayMgr* self = (emscDisplayMgr*) userData;
    o_assert(nullptr != self);

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

} // namespace Render
} // namespace Oryol
