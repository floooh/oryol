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
#include "Resource/ResourceLoader.h"
#include "Resource/SetupAndData.h"
#include "Resource/ResourceInfo.h"
#include "Resource/ResourcePoolInfo.h"

namespace Oryol {

namespace _priv {
class gfxResourceContainer;
class pipeline;
class texture;
class mesh;
}
    
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

    /// subscribe to display events
    static GfxEvent::HandlerId Subscribe(GfxEvent::Handler handler);
    /// unsubscribe from display events
    static void Unsubscribe(GfxEvent::HandlerId id);
    
    /// get the original render setup object
    static const class GfxSetup& GfxSetup();
    /// get the default frame buffer attributes
    static const struct DisplayAttrs& DisplayAttrs();
    /// get the current render pass attributes (default or offscreen)
    static const struct DisplayAttrs& PassAttrs();
    /// get frame-render stats, gets reset in CommitFrame()!
    static const GfxFrameInfo& FrameInfo();

    /// generate new resource label and push on label stack
    static ResourceLabel PushResourceLabel();
    /// push explicit resource label on label stack
    static void PushResourceLabel(ResourceLabel label);
    /// pop resource label from label stack
    static ResourceLabel PopResourceLabel();
    /// create a resource object without associated data
    template<class SETUP> static Id CreateResource(const SETUP& setup);
    /// create a resource object with associated data
    template<class SETUP> static Id CreateResource(const SetupAndData<SETUP>& setupAndData);
    /// create a resource object with associated data
    template<class SETUP> static Id CreateResource(const SETUP& setup, const Buffer& data);
    /// create a resource object with raw pointer to associated data
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

    /// begin rendering to default render pass
    static void BeginPass();
    /// begin rendering to default render pass with override clear values
    static void BeginPass(const PassAction& action);
    /// begin offscreen rendering with override clear colors
    static void BeginPass(const Id& passId, const PassAction& action);
    /// finish rendering to current pass
    static void EndPass();

    /// apply view port
    static void ApplyViewPort(int x, int y, int width, int height, bool originTopLeft=false);
    /// apply scissor rect (must also be enabled in Pipeline object)
    static void ApplyScissorRect(int x, int y, int width, int height, bool originTopLeft=false);
    /// apply draw state (Pipeline, Meshes and Textures)
    static void ApplyDrawState(const DrawState& drawState);
    /// apply a uniform block (call between ApplyDrawState and Draw)
    template<class T> static void ApplyUniformBlock(const T& ub);

    /// update dynamic vertex or index data (complete replace)
    static void UpdateBuffer(const Id& id, const void* data, int numBytes);
    /// update dynamic texture image data (complete replace)
    static void UpdateTexture(const Id& id, const void* data, const ImageDataAttrs& offsetsAndSizes);
    
    /// submit a draw call
    static void Draw(int baseElement, int numElements, int numInstances);

    /// commit (and display) the current frame
    static void CommitFrame();
    /// reset the native 3D-API state-cache
    static void ResetStateCache();

private:
    /// apply uniform block, non-template version
    static void applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, uint32_t layoutHash, const uint8_t* ptr, int byteSize);
};

//------------------------------------------------------------------------------
template<class T> inline void
Gfx::ApplyUniformBlock(const T& ub) {
    applyUniformBlock(T::_bindShaderStage, T::_bindSlotIndex, T::_layoutHash, (const uint8_t*)&ub, sizeof(ub));
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SETUP& setup) {
    o_assert_dbg(IsValid());
    return CreateResource(setup, nullptr, 0);
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SETUP& setup, const Buffer& data) {
    o_assert_dbg(IsValid());
    o_assert_dbg(!data.Empty());
    return CreateResource(setup, data.Data(), data.Size());
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SetupAndData<SETUP>& setupAndData) {
    o_assert_dbg(IsValid());
    return CreateResource(setupAndData.Setup, setupAndData.Data);
}

} // namespace Oryol
