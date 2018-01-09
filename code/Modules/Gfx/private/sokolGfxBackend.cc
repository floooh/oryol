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
Id makeId(GfxResourceType::Code type, uint32_t sgId) {
    // convert a Sokol resource id into a Oryol Id
    Id::SlotIndexT slotIndex = sgId & 0xFFFF;
    Id::UniqueStampT unique = (sgId >> 16) & 0xFFFF;
    Id id(unique, slotIndex, type);
    return id;
}

//------------------------------------------------------------------------------
static void convertPassAction(const PassAction& src, sg_pass_action& dst) {
    o_assert_dbg(GfxConfig::MaxNumColorAttachments <= SG_MAX_COLOR_ATTACHMENTS);
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        for (int c = 0; c < 4; c++) {
            dst.colors[i].val[c] = src.Color[i][c];
        }
        if (src.Flags & (PassAction::ClearC0<<i)) {
            dst.colors[i].action = SG_ACTION_CLEAR;
        }
        else if (src.Flags & (PassAction::LoadC0<<i)) {
            dst.colors[i].action = SG_ACTION_LOAD;
        }
        else {
            dst.colors[i].action = SG_ACTION_DONTCARE;
        }
    }
    dst.depth.val = src.Depth;
    dst.stencil.val = src.Stencil;
    if (src.Flags & PassAction::ClearDS) {
        dst.depth.action = SG_ACTION_CLEAR;
    }
    else if (src.Flags & PassAction::LoadDS) {
        dst.depth.action = SG_ACTION_LOAD;
    }
    else {
        dst.depth.action = SG_ACTION_DONTCARE;
    }
}

//------------------------------------------------------------------------------
static sg_buffer_type convertBufferType(BufferType::Code t) {
    switch (t) {
        case BufferType::VertexBuffer:  return SG_BUFFERTYPE_VERTEXBUFFER;
        case BufferType::IndexBuffer:   return SG_BUFFERTYPE_INDEXBUFFER;
        default: return _SG_BUFFERTYPE_DEFAULT;
    }
}

//------------------------------------------------------------------------------
static sg_usage convertUsage(Usage::Code u) {
    switch (u) {
        case Usage::Immutable:  return SG_USAGE_IMMUTABLE;
        case Usage::Dynamic:    return SG_USAGE_DYNAMIC;
        case Usage::Stream:     return SG_USAGE_STREAM;
        default: return _SG_USAGE_DEFAULT;
    }
}

//------------------------------------------------------------------------------
sokolGfxBackend::~sokolGfxBackend() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::Setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->isValid);
    this->displayManager.SetupDisplay(setup, ptrs);

    // setup sokol-gfx
    sg_desc sgDesc = { };
    sgDesc.buffer_pool_size = setup.ResourcePoolSize[GfxResourceType::Buffer];
    sgDesc.image_pool_size = setup.ResourcePoolSize[GfxResourceType::Texture];
    sgDesc.shader_pool_size = setup.ResourcePoolSize[GfxResourceType::Shader];
    sgDesc.pipeline_pool_size = setup.ResourcePoolSize[GfxResourceType::Pipeline];
    sgDesc.pass_pool_size = setup.ResourcePoolSize[GfxResourceType::RenderPass];
    #if ORYOL_EMSCRIPTEN
    sgDesc.gl_force_gles2 = this->displayManager.force_gles2;
    #elif ORYOL_METAL
    sgDesc.mtl_device = XXX; // FIXME;
    sgDesc.mtl_renderpass_descriptor_cb = XXX; // FIXME
    sgDesc.mtl_drawable_cb = XXX; // FIXME
    #elif ORYOL_D3D11
    sgDesc.d3d11_device = XXX; // FIXME
    sgDesc.d3d11_device_context = XXX; // FIXME
    sgDesc.d3d11_render_target_view_cb = XXX; // FIXME
    sgDesc.d3d11_depth_stencil_view_cb = XXX; // FIXME
    #endif
    sg_setup(&sgDesc);

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
    sg_shutdown();
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
sokolGfxBackend::CreateBuffer(const BufferSetup& setup, const void* data, int dataSize) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(setup.Size <= dataSize);
    sg_buffer_desc sgDesc = { };
    sgDesc.size = setup.Size;
    sgDesc.type = convertBufferType(setup.Type);
    sgDesc.usage = convertUsage(setup.Usage);
    sgDesc.content = data;
    o_assert_dbg(GfxConfig::MaxInflightFrames <= SG_NUM_INFLIGHT_FRAMES);
    #if ORYOL_OPENGL
    for (int i = 0; i < GfxConfig::MaxInflightFrames; i++) {
        sgDesc.gl_buffers[i] = (uint32_t) setup.NativeBuffers[i];
    }
    #elif ORYOL_METAL
    for (int i = 0; i < GfxConfig::MaxInflightFrames; i++) {
        sgDesc.mtl_buffers[i] = (const void*) setup.NativeBuffers[i];
    }
    #elif ORYOL_D3D11
    sgDesc.d3d11_buffer = (const void*) setup.NativeBuffers[0]
    #endif
    return makeId(GfxResourceType::Buffer, sg_make_buffer(&sgDesc).id);
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
    sg_shader_desc sgDesc = { };

    // select the shader language dialect
    ShaderLang::Code slang = ShaderLang::InvalidShaderLang;
    #if ORYOL_OPENGL_CORE_PROFILE
    slang = ShaderLang::GLSL330;
    #elif ORYOL_OPENGL_GLES2
    slang = ShaderLang::GLSL100;
    #elif ORYOL_OPENGL_GLES3
    if (this->displayManager.force_gles2) {
        slang = ShaderLang::GLSL100;
    }
    else {
        slang = ShaderLang::GLSLES3;
    }
    #elif ORYOL_METAL
    slang = ShaderLang::Metal;
    #elif ORYOL_D3D11
    slang = ShaderLang::HLSL5;
    #else
    #error("Unknown Platform")
    #endif

    // set source- or byte-code, and optional entry function
    #if ORYOL_OPENGL
    sgDesc.vs.source = setup.VertexShaderSource(slang).AsCStr();
    sgDesc.fs.source = setup.FragmentShaderSource(slang).AsCStr();
    #elif ORYOL_METAL || ORYOL_D3D11
    const void* byteCodePtr; uint32_t byteCodeSize;
    setup.VertexShaderByteCode(slang, byteCodePtr, byteCodeSize);
    sgDesc.vs.byte_code = byteCodePtr;
    sgDesc.vs.byte_code_size = byteCodeSize;
    setup.FragmentShaderByteCode(slang, byteCodePtr, byteCodeSize);
    sgDesc.fs.byte_code = byteCodePtr;
    sgDesc.fs.byte_code_size = byteCodeSize;
    #endif
    if (setup.VertexShaderFunc(slang).IsValid()) {
        sgDesc.vs.entry = setup.VertexShaderFunc(slang).AsCStr();
    }
    if (setup.FragmentShaderFunc(slang).IsValid()) {
        sgDesc.fs.entry = setup.FragmentShaderFunc(slang).AsCStr();
    }
    // FIXME: uniform blocks and textures
    return makeId(GfxResourceType::Shader, sg_make_shader(&sgDesc).id);
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
    o_assert_dbg(action);
    if (passId.IsValid()) {
        // offscreen framebuffer
    }
    else {
        // default framebuffer
        sg_pass_action sgAction = { };
        convertPassAction(*action, sgAction);
        const DisplayAttrs& attrs = this->displayManager.GetDisplayAttrs();
        sg_begin_default_pass(&sgAction, attrs.FramebufferWidth, attrs.FramebufferHeight);
    }
}

//------------------------------------------------------------------------------
void
sokolGfxBackend::EndPass() {
    o_assert_dbg(this->isValid);
    sg_end_pass();
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
