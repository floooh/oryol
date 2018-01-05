//------------------------------------------------------------------------------
//  sokolGfxBackend.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "sokolGfxBackend.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
sokolGfxBackend::~sokolGfxBackend() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void sokolGfxBackend::Setup(const GfxSetup& setup) {
    o_assert(!this->isValid);
    // FIXME
    this->isValid = true;
}

//------------------------------------------------------------------------------
void sokolGfxBackend::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    // FIXME
}

//------------------------------------------------------------------------------
bool sokolGfxBackend::IsValid() {
    return this->isValid;
}

//------------------------------------------------------------------------------
bool sokolGfxBackend::QuitRequested() {
    // FIXME
    return false;
}

//------------------------------------------------------------------------------
bool sokolGfxBackend::QueryFeature(GfxFeature::Code feature) {
    o_assert_dbg(this->isValid);
    // FIXME!
    return false;
}

//------------------------------------------------------------------------------
GfxEventHandlerId sokolGfxBackend::Subscribe(GfxEventHandler handler) {
    o_assert_dbg(this->isValid);
    // FIXME!
    return GfxEventHandlerId();
}

//------------------------------------------------------------------------------
void sokolGfxBackend::Unsubscribe(GfxEventHandlerId id) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
ResourceLabel sokolGfxBackend::PushResourceLabel() {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::PushResourceLabel(ResourceLabel label) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
ResourceLabel sokolGfxBackend::PopResourceLabel() {
    o_assert_dbg(this->isValid);
    // FIXME
    return ResourceLabel();
}

//------------------------------------------------------------------------------
Id sokolGfxBackend::CreateBuffer(const BufferSetup& setup) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id sokolGfxBackend::CreateImage(const ImageSetup& setup) {
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
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
void sokolGfxBackend::DestroyResource(ResourceLabel label) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::UpdateBuffer(const Id& id, const void* data, int numBytes) {
    o_assert(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::UpdateImage(const Id& id, const ImageContent& data) {
    o_assert(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::BeginPass(Id passId, const PassAction* action) {
    o_assert(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::EndPass() {
    o_assert(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ApplyViewPort(int x, int y, int w, int h, bool originTopLeft) {
    o_assert(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ApplyScissorRect(int x, int y, int w, int h, bool originTopLeft) {
    o_assert(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ApplyDrawState(const DrawState& drawState) {
    o_assert(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ApplyUniformBlock(ShaderStage::Code stage, int ubIndex, const void* data, int numBytes) {
    o_assert(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::Draw(int baseElement, int numElements, int numInstances) {
    o_assert(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::CommitFrame() {
    o_assert(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ResetStateCache() {
    o_assert(this->isValid);
    // FIXME
}

} // namespace _priv
} // namespace Oryol
