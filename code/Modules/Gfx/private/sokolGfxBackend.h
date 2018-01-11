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
    void Setup(const GfxSetup& setup, const gfxPointers& ptrs);
    /// shutdown the Gfx backend
    void Discard();
    /// check if Gfx backend has been setup
    bool IsValid();
    /// return true if the Gfx backend wants to quit the application
    bool QuitRequested();
    /// check if optional feature is supported
    bool QueryFeature(GfxFeature::Code feature);

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
    Id CreateBuffer(const BufferSetup& setup, const void* data, int dataSize);
    /// create (alloc+init) an texture resource
    Id CreateTexture(const TextureSetup& setup, const void* data, int dataSize);
    /// create (alloc+init) a shader resource
    Id CreateShader(const ShaderSetup& setup);
    /// create (alloc+init) a pipeline resource
    Id CreatePipeline(const PipelineSetup& setup);
    /// create (alloc+init) a pass resource
    Id CreatePass(const PassSetup& setup);

    /// allocate a new buffer id
    Id AllocBuffer(const Locator& loc);
    /// allocate a new texture id
    Id AllocTexture(const Locator& loc);
    /// allocate a new shader id
    Id AllocShader(const Locator& loc);
    /// allocate a new pipeline id
    Id AllocPipeline(const Locator& loc);
    /// allocate a new pass id
    Id AllocPass(const Locator& loc);

    /// initialize a buffer
    void InitBuffer(const Id& id, const BufferSetup& setup);
    /// initialize a texture
    void InitTexture(const Id& id, const TextureSetup& setup);
    /// initialize a shader
    void InitShader(const Id& id, const ShaderSetup& setup);
    /// initialize a pipeline
    void InitPipeline(const Id& id, const PipelineSetup& setup);
    /// initialize a pass
    void InitPass(const Id& id, const PassSetup& setup);

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
    void BeginPass(Id passId, const PassAction* action);
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
