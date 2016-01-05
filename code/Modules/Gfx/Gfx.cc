//------------------------------------------------------------------------------
//  Gfx.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx.h"
#include "Core/Core.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {

using namespace _priv;

Gfx::_state* Gfx::state = nullptr;

//------------------------------------------------------------------------------
void
Gfx::Setup(const class GfxSetup& setup) {
    o_assert_dbg(!IsValid());
    state = Memory::New<_state>();
    state->gfxSetup = setup;

    gfxPointers pointers;
    pointers.displayMgr = &state->displayManager;
    pointers.renderer = &state->renderer;
    pointers.resContainer = &state->resourceContainer;
    pointers.meshPool = &state->resourceContainer.meshPool;
    pointers.shaderPool = &state->resourceContainer.shaderPool;
    pointers.texturePool = &state->resourceContainer.texturePool;
    pointers.drawStatePool = &state->resourceContainer.drawStatePool;
    
    state->displayManager.SetupDisplay(setup, pointers);
    state->renderer.setup(setup, pointers);
    state->resourceContainer.setup(setup, pointers);
    state->runLoopId = Core::PreRunLoop()->Add([] {
        state->displayManager.ProcessSystemEvents();
    });
}

//------------------------------------------------------------------------------
void
Gfx::Discard() {
    o_assert_dbg(IsValid());
    state->resourceContainer.Destroy(ResourceLabel::All);
    Core::PreRunLoop()->Remove(state->runLoopId);
    state->renderer.discard();
    state->resourceContainer.discard();
    state->displayManager.DiscardDisplay();
    Memory::Delete(state);
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
Gfx::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
bool
Gfx::QuitRequested() {
    o_assert_dbg(IsValid());
    return state->displayManager.QuitRequested();
}

//------------------------------------------------------------------------------
Gfx::EventHandlerId
Gfx::Subscribe(EventHandler handler) {
    o_assert_dbg(IsValid());
    return state->displayManager.Subscribe(handler);
}

//------------------------------------------------------------------------------
void
Gfx::Unsubscribe(EventHandlerId id) {
    o_assert_dbg(IsValid());
    state->displayManager.Unsubscribe(id);
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
void
Gfx::ApplyDefaultRenderTarget(const ClearState& clearState) {
    o_assert_dbg(IsValid());
    state->renderer.applyRenderTarget(nullptr, clearState);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyRenderTarget(const Id& id, const ClearState& clearState) {
    o_assert_dbg(IsValid());
    o_assert_dbg(id.IsValid());
    o_assert_dbg(id.Type == GfxResourceType::Texture);

    texture* renderTarget = state->resourceContainer.lookupTexture(id);
    o_assert_dbg(nullptr != renderTarget);
    state->renderer.applyRenderTarget(renderTarget, clearState);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyDrawState(const Id& id) {
    o_trace_scoped(Gfx_ApplyDrawState);
    o_assert_dbg(IsValid());
    o_assert_dbg(id.Type == GfxResourceType::DrawState);
    state->renderer.applyDrawState(state->resourceContainer.lookupDrawState(id));
}

//------------------------------------------------------------------------------
bool
Gfx::QueryFeature(GfxFeature::Code feat) {
    o_assert_dbg(IsValid());
    return state->renderer.queryFeature(feat);
}

//------------------------------------------------------------------------------
ResourceLabel
Gfx::PushResourceLabel() {
    o_assert_dbg(IsValid());
    return state->resourceContainer.PushLabel();
}

//------------------------------------------------------------------------------
void
Gfx::PushResourceLabel(ResourceLabel label) {
    o_assert_dbg(IsValid());
    state->resourceContainer.PushLabel(label);
}

//------------------------------------------------------------------------------
ResourceLabel
Gfx::PopResourceLabel() {
    o_assert_dbg(IsValid());
    return state->resourceContainer.PopLabel();
}

//------------------------------------------------------------------------------
Id
Gfx::LoadResource(const Ptr<ResourceLoader>& loader) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.Load(loader);
}

//------------------------------------------------------------------------------
Id
Gfx::LookupResource(const Locator& locator) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.Lookup(locator);
}

//------------------------------------------------------------------------------
int32
Gfx::QueryFreeResourceSlots(GfxResourceType::Code resourceType) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.QueryFreeSlots(resourceType);
}

//------------------------------------------------------------------------------
ResourceInfo
Gfx::QueryResourceInfo(const Id& id) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.QueryResourceInfo(id);
}

//------------------------------------------------------------------------------
ResourcePoolInfo
Gfx::QueryResourcePoolInfo(GfxResourceType::Code resType) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.QueryPoolInfo(resType);
}

//------------------------------------------------------------------------------
void
Gfx::DestroyResources(ResourceLabel label) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.Destroy(label);
}

//------------------------------------------------------------------------------
_priv::gfxResourceContainer&
Gfx::resource() {
    o_assert_dbg(IsValid());
    return state->resourceContainer;
}

//------------------------------------------------------------------------------
void
Gfx::ApplyViewPort(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_assert_dbg(IsValid());
    state->renderer.applyViewPort(x, y, width, height, originTopLeft);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyScissorRect(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_assert_dbg(IsValid());
    state->renderer.applyScissorRect(x, y, width, height, originTopLeft);
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
Gfx::UpdateVertices(const Id& id, const void* data, int32 numBytes) {
    o_trace_scoped(Gfx_UpdateVertices);
    o_assert_dbg(IsValid());
    mesh* msh = state->resourceContainer.lookupMesh(id);
    state->renderer.updateVertices(msh, data, numBytes);
}

//------------------------------------------------------------------------------
void
Gfx::UpdateIndices(const Id& id, const void* data, int32 numBytes) {
    o_trace_scoped(Gfx_UpdateIndices);
    o_assert_dbg(IsValid());
    mesh* msh = state->resourceContainer.lookupMesh(id);
    state->renderer.updateIndices(msh, data, numBytes);
}

//------------------------------------------------------------------------------
void
Gfx::UpdateTexture(const Id& id, const void* data, const ImageDataAttrs& offsetsAndSizes) {
    o_trace_scoped(Gfx_UpdateTexture);
    o_assert_dbg(IsValid());

    #if ORYOL_OPENGL || ORYOL_METAL || ORYOL_D3D11
    texture* tex = state->resourceContainer.lookupTexture(id);
    state->renderer.updateTexture(tex, data, offsetsAndSizes);
    #else
    Log::Warn("Gfx::UpdateTexture(): implement me!\n");
    #endif
}

//------------------------------------------------------------------------------
void
Gfx::ReadPixels(void* buf, int32 bufNumBytes) {
    o_trace_scoped(Gfx_ReadPixels);
    o_assert_dbg(IsValid());
    state->renderer.readPixels(buf, bufNumBytes);
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
