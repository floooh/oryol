//------------------------------------------------------------------------------
//  Gfx.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx.h"
#include "Core/Core.h"

namespace Oryol {

using namespace _priv;

Gfx::_state* Gfx::state = nullptr;

//------------------------------------------------------------------------------
void
Gfx::Setup(const class GfxSetup& setup) {
    o_assert_dbg(!IsValid());
    state = new _state();
    state->gfxSetup = setup;
    state->displayManager.SetupDisplay(setup);
    state->renderer.setup();
    state->resourceManager.Setup(setup, &state->renderer, &state->displayManager);
    state->runLoopId = Core::PreRunLoop()->Add([] {
        state->displayManager.ProcessSystemEvents();
    });
}

//------------------------------------------------------------------------------
void
Gfx::Discard() {
    o_assert_dbg(IsValid());
    Core::PreRunLoop()->Remove(state->runLoopId);
    state->renderer.discard();
    state->resourceManager.Discard();
    state->displayManager.DiscardDisplay();
    delete state;
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
Gfx::QuitRequested() {
    o_assert_dbg(IsValid());
    return state->displayManager.QuitRequested();
}

//------------------------------------------------------------------------------
void
Gfx::AttachEventHandler(const Ptr<Port>& handler) {
    o_assert_dbg(IsValid());
    o_assert_dbg(handler.isValid());
    state->displayManager.AttachDisplayEventHandler(handler);
}

//------------------------------------------------------------------------------
void
Gfx::DetachEventHandler(const Ptr<Port>& handler) {
    o_assert_dbg(IsValid());
    o_assert_dbg(handler.isValid());
    state->displayManager.DetachDisplayEventHandler(handler);
}

//------------------------------------------------------------------------------
const GfxSetup&
Gfx::GfxSetup() {
    o_assert_dbg(IsValid());
    return state->gfxSetup;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
Gfx::DisplayAttrs() {
    o_assert_dbg(IsValid());
    return state->displayManager.GetDisplayAttrs();
}

//------------------------------------------------------------------------------
const DisplayAttrs&
Gfx::RenderTargetAttrs() {
    o_assert_dbg(IsValid());
    return state->renderer.renderTargetAttrs();
}

//------------------------------------------------------------------------------
ResourceState::Code
Gfx::QueryResourceState(const Id& id) {
    o_assert_dbg(IsValid());
    return state->resourceManager.QueryResourceState(id);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyDefaultRenderTarget() {
    o_assert_dbg(IsValid());
    state->renderer.applyRenderTarget(&state->displayManager, nullptr);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyOffscreenRenderTarget(const Id& id) {
    o_assert_dbg(IsValid());
    o_assert_dbg(id.IsValid());

    texture* renderTarget = state->resourceManager.LookupTexture(id);
    o_assert_dbg(nullptr != renderTarget);
    state->renderer.applyRenderTarget(&state->displayManager, renderTarget);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyDrawState(const Id& id) {
    o_trace_scoped(Gfx_ApplyDrawState);
    o_assert_dbg(IsValid());
    state->renderer.applyDrawState(state->resourceManager.LookupDrawState(id));
}

//------------------------------------------------------------------------------
void
Gfx::CommitFrame() {
    o_trace_scoped(Gfx_CommitFrame);
    o_assert_dbg(IsValid());
    state->renderer.commitFrame();
    state->displayManager.Present();
}

//------------------------------------------------------------------------------
void
Gfx::ResetStateCache() {
    o_trace_scoped(Gfx_ResetStateCache);
    o_assert_dbg(IsValid());
    state->renderer.resetStateCache();
}

//------------------------------------------------------------------------------
void
Gfx::UpdateVertices(const Id& id, int32 numBytes, const void* data) {
    o_trace_scoped(Gfx_UpdateVertices);
    o_assert_dbg(IsValid());
    mesh* msh = state->resourceManager.LookupMesh(id);
    state->renderer.updateVertices(msh, numBytes, data);
}

//------------------------------------------------------------------------------
void
Gfx::ReadPixels(void* buf, int32 bufNumBytes) {
    o_trace_scoped(Gfx_ReadPixels);
    o_assert_dbg(IsValid());
    state->renderer.readPixels(&state->displayManager, buf, bufNumBytes);
}

//------------------------------------------------------------------------------
void
Gfx::Clear(PixelChannel::Mask channels, const glm::vec4& color, float32 depth, uint8 stencil) {
    o_assert_dbg(IsValid());
    state->renderer.clear(channels, color, depth, stencil);
}

//------------------------------------------------------------------------------
void
Gfx::Draw(int32 primGroupIndex) {
    o_trace_scoped(Gfx_Draw);
    o_assert_dbg(IsValid());
    state->renderer.draw(primGroupIndex);
}

//------------------------------------------------------------------------------
void
Gfx::Draw(const PrimitiveGroup& primGroup) {
    o_trace_scoped(Gfx_Draw);
    o_assert_dbg(IsValid());
    state->renderer.draw(primGroup);
}

//------------------------------------------------------------------------------
void
Gfx::DrawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_trace_scoped(Gfx_DrawInstanced);
    o_assert_dbg(IsValid());
    state->renderer.drawInstanced(primGroupIndex, numInstances);
}

//------------------------------------------------------------------------------
void
Gfx::DrawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_trace_scoped(Gfx_DrawInstanced);
    o_assert_dbg(IsValid());
    state->renderer.drawInstanced(primGroup, numInstances);
}

} // namespace Oryol
