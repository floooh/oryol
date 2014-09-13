//------------------------------------------------------------------------------
//  Render.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render.h"

namespace Oryol {

using namespace _priv;

Render::_state* Render::state = nullptr;

//------------------------------------------------------------------------------
void
Render::Setup(const RenderSetup& setup) {
    o_assert_dbg(!IsValid());
    state = new _state();
    state->renderSetup = setup;
    state->displayManager.SetupDisplay(setup);
    state->stateWrapper.Setup();
    state->resourceManager.Setup(setup, &state->stateWrapper, &state->displayManager);
    state->renderManager.Setup(&state->stateWrapper, &state->displayManager);
}

//------------------------------------------------------------------------------
void
Render::Discard() {
    o_assert_dbg(IsValid());
    state->renderManager.Discard();
    state->resourceManager.Discard();
    state->stateWrapper.Discard();
    state->displayManager.DiscardDisplay();
    delete state;
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
Render::QuitRequested() {
    o_assert_dbg(IsValid());
    return state->displayManager.QuitRequested();
}

//------------------------------------------------------------------------------
void
Render::AttachEventHandler(const Ptr<Port>& handler) {
    o_assert_dbg(IsValid());
    o_assert_dbg(handler.isValid());
    state->displayManager.AttachDisplayEventHandler(handler);
}

//------------------------------------------------------------------------------
void
Render::DetachEventHandler(const Ptr<Port>& handler) {
    o_assert_dbg(IsValid());
    o_assert_dbg(handler.isValid());
    state->displayManager.DetachDisplayEventHandler(handler);
}

//------------------------------------------------------------------------------
const RenderSetup&
Render::GetRenderSetup() {
    o_assert_dbg(IsValid());
    return state->renderSetup;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
Render::GetDisplayAttrs() {
    o_assert_dbg(IsValid());
    return state->displayManager.GetDisplayAttrs();
}

//------------------------------------------------------------------------------
/**
 NOTE: the LookupResource() method will bump the use-count of looked up
 resource, so make sure to call DiscardResource() when you're done with it!
*/
Id
Render::LookupResource(const Locator& loc) {
    o_assert_dbg(IsValid());
    return state->resourceManager.LookupResource(loc);
}

//------------------------------------------------------------------------------
void
Render::ReleaseResource(const Id& resId) {
    o_assert_dbg(IsValid());
    state->resourceManager.ReleaseResource(resId);
}

//------------------------------------------------------------------------------
ResourceState::Code
Render::QueryResourceState(const Id& resId) {
    o_assert_dbg(IsValid());
    return state->resourceManager.QueryResourceState(resId);
}

//------------------------------------------------------------------------------
void
Render::ApplyDefaultRenderTarget() {
    o_assert_dbg(IsValid());
    state->renderManager.ApplyRenderTarget(nullptr);
}

//------------------------------------------------------------------------------
void
Render::ApplyOffscreenRenderTarget(const Id& resId) {
    o_assert_dbg(IsValid());
    o_assert_dbg(resId.IsValid());

    texture* renderTarget = state->resourceManager.LookupTexture(resId);
    o_assert_dbg(nullptr != renderTarget);
    state->renderManager.ApplyRenderTarget(renderTarget);
}

//------------------------------------------------------------------------------
void
Render::ApplyDrawState(const Id& resId) {
    o_assert_dbg(IsValid());
    state->renderManager.ApplyDrawState(state->resourceManager.LookupDrawState(resId));
}

//------------------------------------------------------------------------------
bool
Render::BeginFrame() {
    o_assert_dbg(IsValid());
    
    state->resourceManager.Update();
    state->displayManager.ProcessSystemEvents();
    state->renderManager.BeginFrame();
    
    /// @todo: check and return whether rendering is possible / necessary
    return true;
}

//------------------------------------------------------------------------------
void
Render::EndFrame() {
    o_assert_dbg(IsValid());
    state->renderManager.EndFrame();
    state->displayManager.Present();
}

//------------------------------------------------------------------------------
void
Render::ResetStateCache() {
    o_assert_dbg(IsValid());
    state->stateWrapper.ResetStateCache();
}

//------------------------------------------------------------------------------
void
Render::UpdateVertices(const Id& resId, int32 numBytes, const void* data) {
    o_assert_dbg(IsValid());
    mesh* msh = state->resourceManager.LookupMesh(resId);
    state->renderManager.UpdateVertices(msh, numBytes, data);
}

//------------------------------------------------------------------------------
void
Render::ReadPixels(void* buf, int32 bufNumBytes) {
    o_assert_dbg(IsValid());
    state->renderManager.ReadPixels(buf, bufNumBytes);
}

//------------------------------------------------------------------------------
void
Render::Clear(PixelChannel::Mask channels, const glm::vec4& color, float32 depth, uint8 stencil) {
    o_assert_dbg(IsValid());
    state->renderManager.Clear(channels, color, depth, stencil);
}

//------------------------------------------------------------------------------
void
Render::Draw(int32 primGroupIndex) {
    o_assert_dbg(IsValid());
    state->renderManager.Draw(primGroupIndex);
}

//------------------------------------------------------------------------------
void
Render::Draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(IsValid());
    state->renderManager.Draw(primGroup);
}

//------------------------------------------------------------------------------
void
Render::DrawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_assert_dbg(IsValid());
    state->renderManager.DrawInstanced(primGroupIndex, numInstances);
}

//------------------------------------------------------------------------------
void
Render::DrawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_assert_dbg(IsValid());
    state->renderManager.DrawInstanced(primGroup, numInstances);
}

} // namespace Oryol
