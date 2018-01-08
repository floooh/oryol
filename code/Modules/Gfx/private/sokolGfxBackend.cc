//------------------------------------------------------------------------------
//  sokolGfxBackend.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"
#define SOKOL_IMPL
#define SOKOL_ASSERT(c) o_assert_dbg(c)
#define SOKOL_MALLOC(s) Oryol::Memory::Alloc(s)
#define SOKOL_FREE(p) Oryol::Memory::Free(p)
#define SOKOL_LOG(m) Oryol::Log::Info("%s\n",m)
#define SOKOL_UNREACHABLE o_assert_dbg(false)
#if ORYOL_OPENGL
    #if ORYOL_RASPBERRYPI
    #define GL_GLEXT_PROTOTYPES
    #include "GLES2/gl2.h"
    #include "GLES2/gl2ext.h"
    #elif ORYOL_WINDOWS || ORYOL_LINUX || ORYOL_MACOS
    #include "Gfx/private/flextGL.h"
    #elif ORYOL_IOS
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
    #elif ORYOL_EMSCRIPTEN
        #if ORYOL_OPENGLES2
        #define GL_GLEXT_PROTOTYPES
        #include <GL/gl.h>
        #include <GL/glext.h>
        #else
        #include <GLES3/gl3.h>
        #endif
    #elif ORYOL_ANDROID
    #define GL_GLEXT_PROTOTYPES
    #include <GLES3/gl3.h>
    #include <GLES3/gl3ext.h>
    #else
    #error "Missing platform for GL header include!"
    #endif
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
void
sokolGfxBackend::Setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->isValid);
    this->displayManager.SetupDisplay(setup, ptrs);
    this->registry.Setup(setup.ResourceRegistryCapacity);
    this->labelStack.Setup(setup.ResourceLabelStackCapacity);
    this->isValid = true;
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::Discard() {
    o_assert(this->isValid);
    this->registry.Discard();
    this->labelStack.Discard();
    this->displayManager.DiscardDisplay();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
sokolGfxBackend::IsValid() {
    return this->isValid;
}

//------------------------------------------------------------------------------
bool
sokolGfxBackend::QuitRequested() {
    return this->displayManager.QuitRequested();
}

//------------------------------------------------------------------------------
bool
sokolGfxBackend::QueryFeature(GfxFeature::Code feature) {
    o_assert_dbg(this->isValid);
    // FIXME
    return false;
}

//------------------------------------------------------------------------------
GfxEvent::HandlerId
sokolGfxBackend::Subscribe(GfxEvent::Handler handler) {
    o_assert_dbg(this->isValid);
    return this->displayManager.Subscribe(handler);
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::Unsubscribe(GfxEvent::HandlerId id) {
    o_assert_dbg(this->isValid);
    this->displayManager.Unsubscribe(id);
}

//------------------------------------------------------------------------------
ResourceLabel
sokolGfxBackend::PushResourceLabel() {
    o_assert_dbg(this->isValid);
    return this->labelStack.PushLabel();
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::PushResourceLabel(ResourceLabel label) {
    o_assert_dbg(this->isValid);
    return this->labelStack.PushLabel(label);
}

//------------------------------------------------------------------------------
ResourceLabel
sokolGfxBackend::PopResourceLabel() {
    o_assert_dbg(this->isValid);
    return this->labelStack.PopLabel();
}

//------------------------------------------------------------------------------
Id
sokolGfxBackend::CreateBuffer(const BufferSetup& setup, const void* data, int size) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id
sokolGfxBackend::CreateTexture(const TextureSetup& setup, const void* data, int size) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id
sokolGfxBackend::CreateShader(const ShaderSetup& setup) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id
sokolGfxBackend::CreatePipeline(const PipelineSetup& setup) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id
sokolGfxBackend::CreatePass(const PassSetup& setup) {
    o_assert_dbg(this->isValid);
    // FIXME
    return Id::InvalidId();
}

//------------------------------------------------------------------------------
Id
sokolGfxBackend::LookupResource(const Locator& loc) {
    o_assert_dbg(this->isValid);
    return this->registry.Lookup(loc);
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::DestroyResources(ResourceLabel label) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::UpdateBuffer(const Id& id, const void* data, int numBytes) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::UpdateTexture(const Id& id, const void* data, const ImageDataAttrs& attrs) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::BeginPass(Id passId, const PassAction* action) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::EndPass() {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::ApplyViewPort(int x, int y, int w, int h, bool originTopLeft) {
    o_assert_dbg(this->isValid);
    sg_apply_viewport(x, y, w, h, originTopLeft);
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::ApplyScissorRect(int x, int y, int w, int h, bool originTopLeft) {
    o_assert_dbg(this->isValid);
    sg_apply_scissor_rect(x, y, w, h, originTopLeft);
}

//------------------------------------------------------------------------------
void sokolGfxBackend::ApplyDrawState(const DrawState& drawState) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::ApplyUniformBlock(ShaderStage::Code stage, int ubIndex, uint32_t layoutHash, const void* data, int numBytes) {
    o_assert_dbg(this->isValid);
    // FIXME
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::Draw(int baseElement, int numElements, int numInstances) {
    o_assert_dbg(this->isValid);
    sg_draw(baseElement, numElements, numInstances);
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::CommitFrame() {
    o_assert_dbg(this->isValid);
    sg_commit();
    this->displayManager.Present();
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::ResetStateCache() {
    o_assert_dbg(this->isValid);
    sg_reset_state_cache();
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::ProcessSystemEvents() {
    o_assert_dbg(this->isValid);
    this->displayManager.ProcessSystemEvents();
}

} // namespace _priv
} // namespace Oryol
