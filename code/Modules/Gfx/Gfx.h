#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Gfx Gfx
    @brief low-level 3D rendering module

    @class Oryol::Gfx
    @ingroup Gfx
    @brief Gfx module facade
*/
#include "Core/RunLoop.h"
#include "Gfx/GfxTypes.h"
#include "Resource/ResourceLabel.h"
#include "Resource/ResourceInfo.h"
#include "Resource/ResourcePoolInfo.h"

namespace Oryol {

class Gfx {
public:
    /// setup Gfx module
    static void Setup(const GfxDesc& desc);
    /// discard Gfx module
    static void Discard();
    /// check if Gfx module is setup
    static bool IsValid();
    
    /// test if the window system wants the application to quit
    static bool QuitRequested();

    /// subscribe to display events
    static GfxEvent::HandlerId Subscribe(GfxEvent::Handler handler);
    /// unsubscribe from display events
    static void Unsubscribe(GfxEvent::HandlerId id);
    
    /// get the original render setup object
    static const GfxDesc& Desc();
    /// get the default frame buffer attributes
    static const struct DisplayAttrs& DisplayAttrs();
    /// get current default framebuffer width
    static int Width();
    /// get current default framebuffer height
    static int Height();
    /// get frame-render stats, gets reset in CommitFrame()!
    static const GfxFrameInfo& FrameInfo();

    /// generate new resource label and push on label stack
    static ResourceLabel PushResourceLabel();
    /// push explicit resource label on label stack
    static void PushResourceLabel(ResourceLabel label);
    /// pop resource label from label stack
    static ResourceLabel PopResourceLabel();

    /// create a buffer object without associated data
    static Id CreateBuffer(const BufferDesc& desc);
    /// create a texture object without associated data
    static Id CreateTexture(const TextureDesc& desc);
    /// create a shader object
    static Id CreateShader(const ShaderDesc& desc);
    /// create a pipeline object
    static Id CreatePipeline(const PipelineDesc& desc);
    /// create a render-pass object
    static Id CreatePass(const PassDesc& desc);

    /// lookup a resource Id by Locator
    static Id LookupResource(const Locator& locator);
    /// destroy one or several resources by matching label
    static void DestroyResources(ResourceLabel label);

    /// allocate a buffer resource id (async resource creation)
    static Id AllocBuffer(const Locator& loc);
    /// initialize a buffer (async resource creation)
    static void InitBuffer(const Id& id, const BufferDesc& desc);
    /// set allocated buffer to failed resource state (async resource creation)
    static void FailBuffer(const Id& id);
    /// allocate a texture resource id (async resource creation)
    static Id AllocTexture(const Locator& loc);
    /// initialize a texture (async resource creation)
    static void InitTexture(const Id& id, const TextureDesc& desc);
    /// set allocated texture to failed resource state (async resource creation)
    static void FailTexture(const Id& id);

    /// test if an optional feature is supported
    static bool QueryFeature(GfxFeature::Code feat);
    /// get the supported shader language
    static ShaderLang::Code QueryShaderLang();
    /// query the resource state of a resource
    static ResourceState::Code QueryResourceState(const Id& id);

    /// begin rendering to default render pass with override clear values
    static void BeginPass(const PassAction& action=PassAction());
    /// begin offscreen rendering with override clear colors
    static void BeginPass(const Id& passId, const PassAction& action=PassAction());
    /// finish rendering to current pass
    static void EndPass();

    /// apply view port
    static void ApplyViewPort(int x, int y, int width, int height, bool originTopLeft=false);
    /// apply scissor rect (must also be enabled in Pipeline object)
    static void ApplyScissorRect(int x, int y, int width, int height, bool originTopLeft=false);
    /// apply pipeline state
    static void ApplyPipeline(const Id& pipId);
    /// apply resource bindings for next draw call (call between ApplyPipeline and Draw)
    static void ApplyBindings(const Bindings& binding);
    /// apply shader uniforms for next draw call (call between ApplyPipeline and Draw)
    template<class T> static void ApplyUniforms(const T& ub);
    /// 'raw form' of ApplyUniforms, with explicit shader state, uniform buffer slot and data ptr/size
    static void ApplyUniforms(ShaderStage::Code stage, int slot, const void* data, int numBytes);

    /// update dynamic vertex or index data (complete replace)
    static void UpdateBuffer(const Id& id, const void* data, int numBytes);
    /// update dynamic texture image data (complete replace)
    static void UpdateTexture(const Id& id, const ImageContent& content);
    
    /// submit a draw call
    static void Draw(int baseElement, int numElements, int numInstances=1);
    /// submit a draw call with baseElement and numElements taken from PrimitiveGroup
    static void Draw(const PrimitiveGroup& primGroup, int numInstances=1);

    /// commit (and display) the current frame
    static void CommitFrame();
    /// reset the native 3D-API state-cache
    static void ResetStateCache();
};

//------------------------------------------------------------------------------
template<class T> inline void
Gfx::ApplyUniforms(const T& ub) {
    ApplyUniforms(T::_bindShaderStage, T::_bindSlotIndex, (const uint8_t*)&ub, sizeof(ub));
}

} // namespace Oryol
