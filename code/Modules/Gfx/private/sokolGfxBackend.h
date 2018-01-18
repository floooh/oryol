#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::sokolGfxBackend
    @ingroup _priv
    @brief graphics backend implementation on top of sokol_gfx.h
*/
#include "Gfx/GfxTypes.h"
#include "Gfx/private/displayMgr.h"
#include "Gfx/private/gfxPointers.h"
#include "Resource/ResourceLabel.h"
#include "Resource/ResourceRegistry.h"
#include "Resource/ResourceLabelStack.h"
#include "sokol_gfx.h"

namespace Oryol {
namespace _priv {

class sokolGfxBackend {
public:
    /// destructor
    ~sokolGfxBackend();

    /// setup the Gfx backend
    void Setup(const GfxDesc& desc, const gfxPointers& ptrs);
    /// shutdown the Gfx backend
    void Discard();
    /// check if Gfx backend has been setup
    bool IsValid();
    /// return true if the Gfx backend wants to quit the application
    bool QuitRequested();
    /// check if optional feature is supported
    bool QueryFeature(GfxFeature::Code feature);
    /// query the supported shader language
    ShaderLang::Code QueryShaderLang();

    /// subscribe to display events
    GfxEvent::HandlerId Subscribe(GfxEvent::Handler handler);
    /// unsubscribe from display events
    void Unsubscribe(GfxEvent::HandlerId id);

    /// generate new resource label and push on label stack
    ResourceLabel PushResourceLabel();
    /// push explicit resource label on label stack
    void PushResourceLabel(ResourceLabel label);
    /// pop resource label from label stack
    ResourceLabel PopResourceLabel();
    
    /// create (alloc+init) a buffer resource
    Id CreateBuffer(const BufferDesc& desc, const void* data, int dataSize);
    /// create (alloc+init) an texture resource
    Id CreateTexture(const TextureDesc& desc, const void* data, int dataSize);
    /// create (alloc+init) a shader resource
    Id CreateShader(const ShaderDesc& desc);
    /// create (alloc+init) a pipeline resource
    Id CreatePipeline(const PipelineDesc& desc);
    /// create (alloc+init) a pass resource
    Id CreatePass(const PassDesc& desc);

    /// allocate a new buffer id
    Id AllocBuffer();
    /// allocate a new texture id
    Id AllocTexture();
    /// initialize a buffer
    void InitBuffer(const Id& id, const BufferDesc& desc, const void* data, int dataSize);
    /// initialize a texture
    void InitTexture(const Id& id, const TextureDesc& desc, const void* data, int dataSize);
    /// set allocated, non-initialized buffer to failed resource state
    void FailBuffer(const Id& id);
    /// set allocated, non-initialized texture to failed resource state
    void FailTexture(const Id& id);

    /// lookup a resource Id by locator
    Id LookupResource(const Locator& loc);
    /// add a shared resource to the resource registry
    void AddResource(const Locator& loc, const Id& id);
    /// destroy one or multiple resource(s) by matching label
    void DestroyResources(ResourceLabel label);

    /// update dynamic buffer data
    void UpdateBuffer(const Id& id, const void* data, int numBytes);
    /// update dynamic texture data
    void UpdateTexture(const Id& id, const void* data, const ImageDataAttrs& attrs);

    /// begin rendering pass
    void BeginPass(const Id& passId, const PassAction* action);
    /// finish rendering pass
    void EndPass();

    /// apply viewport
    void ApplyViewPort(int x, int y, int w, int h, bool originTopLeft);
    /// apply scissor rect
    void ApplyScissorRect(int x, int y, int w, int h, bool originTopLeft);
    /// apply a draw state
    void ApplyDrawState(const DrawState& drawState);
    /// apply a uniform block
    void ApplyUniformBlock(ShaderStage::Code stage, int ubIndex, const void* data, int numBytes);

    /// issue a draw call
    void Draw(int baseElement, int numElements, int numInstances);

    /// commit current frame
    void CommitFrame();
    /// present current frame
    void Preset();
    /// reset the internal state cache
    void ResetStateCache();
    /// process window system events
    void ProcessSystemEvents();

    bool isValid = false;
    displayMgr displayManager;
    ResourceRegistry registry;
    ResourceLabelStack labelStack;
};

} // namespace _priv
} // namespace Oryol
