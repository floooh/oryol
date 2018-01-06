//------------------------------------------------------------------------------
//  sokolGfxBackend.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"
#define SOKOL_IMPL
#define SOKOL_ASSERT(c) o_assert_dbg(c)
#define SOKOL_MALLOC(s) Oryol::Memory::Alloc(s)
#define SOKOL_FREE(p) Oryol::Memory::Free(p)
#define SOKOL_LOG(m) Oryol::Log::Info("%s\n",m)
#define SOKOL_UNREACHABLE o_assert_dbg(false)
#if ORYOL_OPENGL
#include "Gfx/private/gl/gl_impl.h"
#endif
#if ORYOL_OPENGLES2
#define SOKKOL_GLES2
#elif ORYOL_OPENGL3
#define SOKOL_GLES3
#elif ORYOL_OPENGL_CORE_PROFILE
#define SOKOL_GLCORE33
#elif ORYOL_D3D11
#define SOKOL_D3D11
#elif ORYOL_METAL
#if ORYOL_MACOS
#define SOKOL_METAL_MACOS
#else
#define SOKOL_METAL_IOS
#endif
#endif
#include "sokolGfxBackend.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
sokolGfxBackend::~sokolGfxBackend() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void sokolGfxBackend::Setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->isValid);
    this->displayManager.SetupDisplay(setup, ptrs);
    this->resourceContainer.setup(setup, ptrs);
    this->renderer.setup(setup, ptrs);
    this->isValid = true;
}

//------------------------------------------------------------------------------
void sokolGfxBackend::Discard() {
    o_assert(this->isValid);
    this->resourceContainer.GarbageCollect();
    this->resourceContainer.Destroy(ResourceLabel::All);
    this->renderer.discard();
    this->resourceContainer.discard();
    this->displayManager.DiscardDisplay();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool sokolGfxBackend::IsValid() {
    return this->isValid;
}

//------------------------------------------------------------------------------
bool sokolGfxBackend::QuitRequested() {
    return this->displayManager.QuitRequested();
}

//------------------------------------------------------------------------------
bool sokolGfxBackend::QueryFeature(GfxFeature::Code feature) {
    o_assert_dbg(this->isValid);
    return this->renderer.queryFeature(feature);
}

//------------------------------------------------------------------------------
GfxEvent::HandlerId sokolGfxBackend::Subscribe(GfxEvent::Handler handler) {
    o_assert_dbg(this->isValid);
    return this->displayManager.Subscribe(handler);
}

//------------------------------------------------------------------------------
void sokolGfxBackend::Unsubscribe(GfxEvent::HandlerId id) {
    o_assert_dbg(this->isValid);
    this->displayManager.Unsubscribe(id);
}

//------------------------------------------------------------------------------
ResourceLabel sokolGfxBackend::PushResourceLabel() {
    o_assert_dbg(this->isValid);
    return this->resourceContainer.labelStack.PushLabel();
}

//------------------------------------------------------------------------------
void sokolGfxBackend::PushResourceLabel(ResourceLabel label) {
    o_assert_dbg(this->isValid);
    return this->resourceContainer.labelStack.PushLabel(label);
}

//------------------------------------------------------------------------------
ResourceLabel sokolGfxBackend::PopResourceLabel() {
    o_assert_dbg(this->isValid);
    return this->resourceContainer.labelStack.PopLabel();
}

//------------------------------------------------------------------------------
Id sokolGfxBackend::CreateBuffer(const BufferSetup& setup) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id sokolGfxBackend::CreateTexture(const TextureSetup& setup) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id sokolGfxBackend::CreateShader(const ShaderSetup& setup) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id sokolGfxBackend::CreatePipeline(const PipelineSetup& setup) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id sokolGfxBackend::CreatePass(const PassSetup& setup) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id sokolGfxBackend::LookupResource(const Locator& loc) {
    o_assert_dbg(this->isValid);
    return this->resourceContainer.registry.Lookup(loc);
}

//------------------------------------------------------------------------------
void sokolGfxBackend::DestroyResources(ResourceLabel label) {
    o_assert_dbg(this->isValid);
    return this->resourceContainer.DestroyDeferred(label);
}

//------------------------------------------------------------------------------
void sokolGfxBackend::UpdateBuffer(const Id& id, const void* data, int numBytes) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::UpdateTexture(const Id& id, const ImageDataAttrs& attrs, const void* data, int numBytes) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::BeginPass(Id passId, const PassAction* action) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::EndPass() {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ApplyViewPort(int x, int y, int w, int h, bool originTopLeft) {
    o_assert_dbg(this->isValid);
    this->renderer.applyViewPort(x, y, w, h, originTopLeft);
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ApplyScissorRect(int x, int y, int w, int h, bool originTopLeft) {
    o_assert_dbg(this->isValid);
    this->renderer.applyScissorRect(x, y, w, h, originTopLeft);
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ApplyDrawState(const DrawState& drawState) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ApplyUniformBlock(ShaderStage::Code stage, int ubIndex, const void* data, int numBytes) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::Draw(int baseElement, int numElements, int numInstances) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::CommitFrame() {
    o_assert_dbg(this->isValid);
    this->renderer.commitFrame();
    this->displayManager.Present();
    this->resourceContainer.GarbageCollect();
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ResetStateCache() {
    o_assert_dbg(this->isValid);
    this->renderer.resetStateCache();
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ProcessSystemEvents() {
    o_assert_dbg(this->isValid);
    this->displayManager.ProcessSystemEvents();
}

} // namespace _priv
} // namespace Oryol
