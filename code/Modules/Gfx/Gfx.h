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
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/gfxResourceContainer.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Core/ClearState.h"
#include "Gfx/Core/DrawState.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Core/GfxFrameInfo.h"
#include "Resource/Core/SetupAndData.h"
#include "glm/vec4.hpp"

namespace Oryol {
    
class Gfx {
public:
    /// setup Gfx module
    static void Setup(const GfxSetup& setup);
    /// discard Gfx module
    static void Discard();
    /// check if Gfx module is setup
    static bool IsValid();
    
    /// test if the window system wants the application to quit
    static bool QuitRequested();

    /// event handler callback typedef
    typedef _priv::displayMgrBase::eventHandler EventHandler;
    /// event handler id typedef
    typedef _priv::displayMgrBase::eventHandlerId EventHandlerId;
    /// subscribe to display events
    static EventHandlerId Subscribe(EventHandler handler);
    /// unsubscribe from display events
    static void Unsubscribe(EventHandlerId id);
    
    /// get the original render setup object
    static const class GfxSetup& GfxSetup();
    /// get the default frame buffer attributes
    static const struct DisplayAttrs& DisplayAttrs();
    /// get the current render target attributes (default or offscreen)
    static const struct DisplayAttrs& RenderTargetAttrs();
    /// get frame-render stats, gets reset in CommitFrame()!
    static const GfxFrameInfo& FrameInfo();

    /// generate new resource label and push on label stack
    static ResourceLabel PushResourceLabel();
    /// push explicit resource label on label stack
    static void PushResourceLabel(ResourceLabel label);
    /// pop resource label from label stack
    static ResourceLabel PopResourceLabel();
    /// create a resource object
    template<class SETUP> static Id CreateResource(const SETUP& setup);
    /// create a resource object with data in buffer object
    template<class SETUP> static Id CreateResource(const SetupAndData<SETUP>& setupAndData);
    /// create a resource object with data in buffer object
    template<class SETUP> static Id CreateResource(const SETUP& setup, const Buffer& data);
    /// create a resource object with pointer to non-owned data
    template<class SETUP> static Id CreateResource(const SETUP& setup, const void* data, int size);
    /// asynchronously load resource object
    static Id LoadResource(const Ptr<ResourceLoader>& loader);
    /// lookup a resource Id by Locator
    static Id LookupResource(const Locator& locator);
    /// destroy one or several resources by matching label
    static void DestroyResources(ResourceLabel label);

    /// test if an optional feature is supported
    static bool QueryFeature(GfxFeature::Code feat);
    /// query number of free slots for resource type
    static int QueryFreeResourceSlots(GfxResourceType::Code resourceType);
    /// query resource info (fast)
    static ResourceInfo QueryResourceInfo(const Id& id);
    /// query resource pool info (slow)
    static ResourcePoolInfo QueryResourcePoolInfo(GfxResourceType::Code resType);

    /// make the default render target current and optionally clear
    static void ApplyDefaultRenderTarget(const ClearState& clearState=ClearState());
    /// apply an offscreen render target
    static void ApplyRenderTarget(const Id& id, const ClearState& clearState=ClearState());
    /// apply view port
    static void ApplyViewPort(int x, int y, int width, int height, bool originTopLeft=false);
    /// apply scissor rect (must also be enabled in DrawState.RasterizerState)
    static void ApplyScissorRect(int x, int y, int width, int height, bool originTopLeft=false);
    /// apply draw state and meshes to use for rendering without texture blocks
    static void ApplyDrawState(const DrawState& drawState);
    /// apply a uniform block
    template<class T> static void ApplyUniformBlock(const T& ub);

    /// update dynamic vertex data (only complete replace possible at the moment)
    static void UpdateVertices(const Id& id, const void* data, int numBytes);
    /// update dynamic index data (only complete replace possible at the moment)
    static void UpdateIndices(const Id& id, const void* data, int numBytes);
    /// update dynamic texture image data (only complete replace possible at the moment)
    static void UpdateTexture(const Id& id, const void* data, const ImageDataAttrs& offsetsAndSizes);
    /// read current framebuffer pixels into client memory, this means a PIPELINE STALL!!
    static void ReadPixels(void* ptr, int numBytes);
    
    /// submit a draw call with primitive group index in current mesh
    static void Draw(int primGroupIndex);
    /// submit a draw call with direct primitive group
    static void Draw(const PrimitiveGroup& primGroup);
    /// submit a draw call for instanced rendering
    static void DrawInstanced(int primGroupIndex, int numInstances);
    /// submit a draw call for instanced rendering with direct primitive group
    static void DrawInstanced(const PrimitiveGroup& primGroup, int numInstances);

    /// commit (and display) the current frame
    static void CommitFrame();
    /// reset internal state (must be called when directly rendering through the native 3D API)
    static void ResetStateCache();

    /// direct access to resource container (private interface for resource loaders)
    static _priv::gfxResourceContainer& resource();

private:
    #if ORYOL_DEBUG
    /// validate mesh binding
    static void validateMeshes(_priv::pipeline* pip, _priv::mesh** meshes, int numMeshes);
    /// validate texture binding
    static void validateTextures(ShaderStage::Code stage, _priv::pipeline* pip, _priv::texture** textures, int numTextures);
    #endif
    /// private generic apply texture block method
    template<class T> static void applyTextureBlock(const T& tb);

    // FIXME: _state should be forward-declared, so that Gfx.h doesn't need to include all these headers
    struct _state {
        class GfxSetup gfxSetup;
        GfxFrameInfo gfxFrameInfo;
        RunLoop::Id runLoopId = RunLoop::InvalidId;
        _priv::displayMgr displayManager;
        class _priv::renderer renderer;
        _priv::gfxResourceContainer resourceContainer;
    };
    static _state* state;
};

//------------------------------------------------------------------------------
template<class T> inline void
Gfx::ApplyUniformBlock(const T& ub) {
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumApplyUniformBlock++;
    state->renderer.applyUniformBlock(T::_bindShaderStage, T::_bindSlotIndex, T::_layoutHash, (const uint8_t*) &ub, sizeof(ub));
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SETUP& setup) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.Create(setup);
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SETUP& setup, const Buffer& data) {
    o_assert_dbg(IsValid());
    o_assert_dbg(!data.Empty());
    return state->resourceContainer.Create(setup, data.Data(), data.Size());
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SetupAndData<SETUP>& setupAndData) {
    o_assert_dbg(IsValid());
    return CreateResource(setupAndData.Setup, setupAndData.Data);
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SETUP& setup, const void* data, int size) {
    o_assert_dbg(IsValid());
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);
    return state->resourceContainer.Create(setup, data, size);
}

} // namespace Oryol
