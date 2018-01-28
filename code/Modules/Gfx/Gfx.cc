//------------------------------------------------------------------------------
//  Gfx.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx.h"
#include "Core/Core.h"
#include "Core/Trace.h"
#include "Gfx/private/gfxBackend.h"

namespace Oryol {

using namespace _priv;

namespace {
    struct _state {
        GfxDesc gfxDesc;
        GfxFrameInfo gfxFrameInfo;
        RunLoop::Id runLoopId = RunLoop::InvalidId;
        _priv::gfxBackend backend;
        bool inPass = false;
    };
    _state* state = nullptr;
}

//------------------------------------------------------------------------------
void
Gfx::Setup(const GfxDesc& desc) {
    o_assert_dbg(!IsValid());
    state = Memory::New<_state>();
    state->gfxDesc = desc;

    state->backend.Setup(desc);
    state->runLoopId = Core::PreRunLoop()->Add([] {
        state->backend.ProcessSystemEvents();
    });
    state->gfxFrameInfo = GfxFrameInfo();
}

//------------------------------------------------------------------------------
void
Gfx::Discard() {
    o_assert_dbg(IsValid());
    o_assert_dbg(!state->inPass);
    Core::PreRunLoop()->Remove(state->runLoopId);
    state->backend.Discard();
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
    return state->backend.QuitRequested();
}

//------------------------------------------------------------------------------
GfxEvent::HandlerId
Gfx::Subscribe(GfxEvent::Handler handler) {
    o_assert_dbg(IsValid());
    return state->backend.Subscribe(handler);
}

//------------------------------------------------------------------------------
void
Gfx::Unsubscribe(GfxEvent::HandlerId id) {
    o_assert_dbg(IsValid());
    state->backend.Unsubscribe(id);
}

//------------------------------------------------------------------------------
const GfxDesc&
Gfx::Desc() {
    o_assert_dbg(IsValid());
    return state->gfxDesc;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
Gfx::DisplayAttrs() {
    o_assert_dbg(IsValid());
    return state->backend.displayManager.GetDisplayAttrs();
}

//------------------------------------------------------------------------------
const DisplayAttrs&
Gfx::PassAttrs() {
    o_assert_dbg(IsValid());
    // FIXME
    return DisplayAttrs();
    //return state->backend.renderer.renderPassAttrs();
}

//------------------------------------------------------------------------------
const GfxFrameInfo&
Gfx::FrameInfo() {
    o_assert_dbg(IsValid());
    return state->gfxFrameInfo;
}

//------------------------------------------------------------------------------
void
Gfx::BeginPass(const PassAction& action) {
    o_assert_dbg(IsValid());
    o_assert_dbg(!state->inPass);
    state->inPass = true;
    state->gfxFrameInfo.NumPasses++;
    state->backend.BeginPass(Id::InvalidId(), &action);
}

//------------------------------------------------------------------------------
void
Gfx::BeginPass(const Id& id, const PassAction& passAction) {
    o_assert_dbg(IsValid());
    o_assert_dbg(!state->inPass);
    state->inPass = true;
    state->gfxFrameInfo.NumPasses++;
    state->backend.BeginPass(id, &passAction);
}

//------------------------------------------------------------------------------
void
Gfx::EndPass() {
    o_assert_dbg(IsValid());
    o_assert_dbg(state->inPass);
    state->inPass = false;
    state->backend.EndPass();
}

//------------------------------------------------------------------------------
void
Gfx::ApplyDrawState(const DrawState& drawState) {
    o_trace_scoped(Gfx_ApplyDrawState);
    o_assert_dbg(IsValid());
    o_assert_dbg(state->inPass);
    state->gfxFrameInfo.NumApplyDrawState++;
    state->backend.ApplyDrawState(drawState);
}

//------------------------------------------------------------------------------
bool
Gfx::QueryFeature(GfxFeature::Code feat) {
    o_assert_dbg(IsValid());
    return state->backend.QueryFeature(feat);
}

//------------------------------------------------------------------------------
ShaderLang::Code
Gfx::QueryShaderLang() {
    o_assert_dbg(IsValid());
    return state->backend.QueryShaderLang();
}

//------------------------------------------------------------------------------
ResourceState::Code
Gfx::QueryResourceState(const Id& id) {
    o_assert_dbg(IsValid());
    return state->backend.QueryResourceState(id);
}

//------------------------------------------------------------------------------
ResourceLabel
Gfx::PushResourceLabel() {
    o_assert_dbg(IsValid());
    return state->backend.PushResourceLabel();
}

//------------------------------------------------------------------------------
void
Gfx::PushResourceLabel(ResourceLabel label) {
    o_assert_dbg(IsValid());
    state->backend.PushResourceLabel(label);
}

//------------------------------------------------------------------------------
ResourceLabel
Gfx::PopResourceLabel() {
    o_assert_dbg(IsValid());
    return state->backend.PopResourceLabel();
}

//------------------------------------------------------------------------------
Id
Gfx::LookupResource(const Locator& locator) {
    o_assert_dbg(IsValid());
    return state->backend.LookupResource(locator);
}

//------------------------------------------------------------------------------
void
Gfx::DestroyResources(ResourceLabel label) {
    o_assert_dbg(IsValid());
    return state->backend.DestroyResources(label);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyViewPort(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(IsValid());
    o_assert_dbg(state->inPass);
    state->gfxFrameInfo.NumApplyViewPort++;
    state->backend.ApplyViewPort(x, y, width, height, originTopLeft);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyScissorRect(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(IsValid());
    o_assert_dbg(state->inPass);
    state->gfxFrameInfo.NumApplyScissorRect++;
    state->backend.ApplyScissorRect(x, y, width, height, originTopLeft);
}

//------------------------------------------------------------------------------
void
Gfx::CommitFrame() {
    o_trace_scoped(Gfx_CommitFrame);
    o_assert_dbg(IsValid());
    o_assert_dbg(!state->inPass);
    state->backend.CommitFrame();
    state->gfxFrameInfo = GfxFrameInfo();
}

//------------------------------------------------------------------------------
void
Gfx::ResetStateCache() {
    o_trace_scoped(Gfx_ResetStateCache);
    o_assert_dbg(IsValid());
    state->backend.ResetStateCache();
}

//------------------------------------------------------------------------------
void
Gfx::UpdateBuffer(const Id& id, const void* data, int numBytes) {
    o_trace_scoped(Gfx_UpdateBuffer);
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumUpdateBuffers++;
    state->backend.UpdateBuffer(id, data, numBytes);
}

//------------------------------------------------------------------------------
void
Gfx::UpdateTexture(const Id& id, const ImageContent& content) {
    o_trace_scoped(Gfx_UpdateTexture);
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumUpdateTextures++;
    state->backend.UpdateTexture(id, content);
}

//------------------------------------------------------------------------------
void
Gfx::Draw(int baseElement, int numElements, int numInstances) {
    o_trace_scoped(Gfx_Draw);
    o_assert_dbg(IsValid());
    o_assert_dbg(state->inPass);
    state->gfxFrameInfo.NumDraw++;
    state->backend.Draw(baseElement, numElements, numInstances);
}

//------------------------------------------------------------------------------
void
Gfx::Draw(const PrimitiveGroup& primGroup, int numInstances) {
    o_trace_scoped(Gfx_Draw);
    o_assert_dbg(IsValid());
    o_assert_dbg(state->inPass);
    state->gfxFrameInfo.NumDraw++;
    state->backend.Draw(primGroup.BaseElement, primGroup.NumElements, numInstances);
}

//------------------------------------------------------------------------------
Id
Gfx::CreateTexture(const TextureDesc& desc) {
    o_assert_dbg(IsValid());
    Id resId = state->backend.LookupResource(desc.Locator);
    if (!resId.IsValid()) {
        resId = state->backend.CreateTexture(desc);
        state->backend.AddResource(desc.Locator, resId);
    }
    return resId;
}

//------------------------------------------------------------------------------
Id
Gfx::CreateBuffer(const BufferDesc& desc) {
    o_assert_dbg(IsValid());
    Id resId = state->backend.LookupResource(desc.Locator);
    if (!resId.IsValid()) {
        resId = state->backend.CreateBuffer(desc);
        state->backend.AddResource(desc.Locator, resId);
    }
    return resId;
}

//------------------------------------------------------------------------------
Id
Gfx::CreateShader(const ShaderDesc& desc) {
    o_assert_dbg(IsValid());
    Id resId = state->backend.LookupResource(desc.Locator);
    if (!resId.IsValid()) {
        resId = state->backend.CreateShader(desc);
        state->backend.AddResource(desc.Locator, resId);
    }
    return resId;
}

//------------------------------------------------------------------------------
Id
Gfx::CreatePipeline(const PipelineDesc& desc) {
    o_assert_dbg(IsValid());
    Id resId = state->backend.LookupResource(desc.Locator);
    if (!resId.IsValid()) {
        resId = state->backend.CreatePipeline(desc);
        state->backend.AddResource(desc.Locator, resId);
    }
    return resId;
}

//------------------------------------------------------------------------------
Id
Gfx::CreatePass(const PassDesc& desc) {
    o_assert_dbg(IsValid());
    Id resId = state->backend.LookupResource(desc.Locator);
    if (!resId.IsValid()) {
        resId = state->backend.CreatePass(desc);
        state->backend.AddResource(desc.Locator, resId);
    }
    return resId;
}

//------------------------------------------------------------------------------
Id
Gfx::AllocBuffer(const Locator& loc) {
    o_assert_dbg(IsValid());
    Id resId = state->backend.LookupResource(loc);
    if (!resId.IsValid()) {
        resId = state->backend.AllocBuffer();
        state->backend.AddResource(loc, resId);
    }
    return resId;
}

//------------------------------------------------------------------------------
Id
Gfx::AllocTexture(const Locator& loc) {
    o_assert_dbg(IsValid());
    Id resId = state->backend.LookupResource(loc);
    if (!resId.IsValid()) {
        resId = state->backend.AllocTexture();
        state->backend.AddResource(loc, resId);
    }
    return resId;
}

//------------------------------------------------------------------------------
void
Gfx::InitBuffer(const Id& id, const BufferDesc& desc) {
    o_assert_dbg(IsValid());
    state->backend.InitBuffer(id, desc);
}

//------------------------------------------------------------------------------
void
Gfx::InitTexture(const Id& id, const TextureDesc& desc) {
    o_assert_dbg(IsValid());
    state->backend.InitTexture(id, desc);
}

//------------------------------------------------------------------------------
void
Gfx::FailBuffer(const Id& id) {
    o_assert_dbg(IsValid());
    state->backend.FailBuffer(id);
}

//------------------------------------------------------------------------------
void
Gfx::FailTexture(const Id& id) {
    o_assert_dbg(IsValid());
    state->backend.FailTexture(id);
}

//------------------------------------------------------------------------------
void
Gfx::applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, const uint8_t* ptr, int byteSize) {
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumApplyUniformBlock++;
    state->backend.ApplyUniformBlock(bindStage, bindSlot, ptr, byteSize);
}

} // namespace Oryol
