#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Gfx Gfx
    @brief 3D rendering

    @class Oryol::Gfx
    @ingroup Gfx
    @brief Gfx module facade
*/
#include "Core/RunLoop.h"
#include "Gfx/Core/displayMgr.h"
#include "IO/Stream/Stream.h"
#include "Resource/GfxResourceContainer.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Setup/MeshSetup.h"
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
    
    /// attach a display event handler
    static void AttachEventHandler(const Ptr<Port>& handler);
    /// detach a display event handler
    static void DetachEventHandler(const Ptr<Port>& handler);
    
    /// get the original render setup object
    static const class GfxSetup& GfxSetup();
    /// get the default frame buffer attributes
    static const struct DisplayAttrs& DisplayAttrs();
    /// get the current render target attributes (default or offscreen)
    static const struct DisplayAttrs& RenderTargetAttrs();
    /// test if an optional feature is supported
    static bool Supports(GfxFeature::Code feat);
    
    /// generate new resource label and push on label stack
    static ResourceLabel PushResourceLabel();
    /// push explicit resource label on label stack
    static void PushResourceLabel(ResourceLabel label);
    /// pop resource label from label stack
    static ResourceLabel PopResourceLabel();
    /// create a resource object
    template<class SETUP> static Id CreateResource(const SETUP& setup);
    /// create a resource object with data in stream object
    template<class SETUP> static Id CreateResource(const SetupAndStream<SETUP>& setupAndStream);
    /// create a resource object with data in stream object
    template<class SETUP> static Id CreateResource(const SETUP& setup, const Ptr<Stream>& stream);
    /// create a resource object with pointer to non-owned data
    template<class SETUP> static Id CreateResource(const SETUP& setup, const void* data, int32 size);
    /// asynchronously load resource object
    static Id LoadResource(const Ptr<ResourceLoader>& loader);
    /// lookup a resource Id by Locator
    static Id LookupResource(const Locator& locator);
    /// query number of free slots for resource type
    static int32 QueryFreeResourceSlots(GfxResourceType::Code resourceType);
    /// query resource info (fast)
    static ResourceInfo QueryResourceInfo(const Id& id);
    /// query resource pool info (slow)
    static ResourcePoolInfo QueryResourcePoolInfo(GfxResourceType::Code resType);
    /// destroy one or several resources by matching label
    static void DestroyResources(ResourceLabel label);

    /// make the default render target (backbuffer) current
    static void ApplyDefaultRenderTarget();
    /// apply an offscreen render target
    static void ApplyOffscreenRenderTarget(const Id& id);
    /// apply view port
    static void ApplyViewPort(int32 x, int32 y, int32 width, int32 height);
    /// apply scissor rect (must also be enabled in DrawState.RasterizerState)
    static void ApplyScissorRect(int32 x, int32 y, int32 width, int32 height);
    /// apply blend color (see DrawState.BlendState)
    static void ApplyBlendColor(const glm::vec4& blendColor);
    /// apply draw state to use for rendering
    static void ApplyDrawState(const Id& id);
    /// apply a shader constant block
    static void ApplyConstantBlock(const Id& id);
    /// apply a shader variable
    template<class T> static void ApplyVariable(int32 index, const T& value);
    /// apply a shader variable array
    template<class T> static void ApplyVariableArray(int32 index, const T* values, int32 numValues);
    
    /// update dynamic vertex data (only complete replace possible at the moment)
    static void UpdateVertices(const Id& id, const void* data, int32 numBytes);
    /// update dynamic index data (only complete replace possible at the moment)
    static void UpdateIndices(const Id& id, const void* data, int32 numBytes);
    /// read current framebuffer pixels into client memory, this means a PIPELINE STALL!!
    static void ReadPixels(void* ptr, int32 numBytes);
    
    /// clear the currently assigned render target (default depth value is 1.0f, default stencil value is 0)
    static void Clear(PixelChannel::Mask channels, const glm::vec4& color, float32 depth=1.0f, uint8 stencil=0);
    /// submit a draw call with primitive group index in current mesh
    static void Draw(int32 primGroupIndex);
    /// submit a draw call with direct primitive group
    static void Draw(const PrimitiveGroup& primGroup);
    /// submit a draw call for instanced rendering
    static void DrawInstanced(int32 primGroupIndex, int32 numInstances);
    /// submit a draw call for instanced rendering with direct primitive group
    static void DrawInstanced(const PrimitiveGroup& primGroup, int32 numInstances);

    /// commit (and display) the current frame
    static void CommitFrame();
    /// reset internal state (must be called when directly rendering through GL; FIXME: better name?)
    static void ResetStateCache();

    /// direct access to resource container (private interface for resource loaders)
    static GfxResourceContainer& resource();

private:
    struct _state {
        class GfxSetup gfxSetup;
        RunLoop::Id runLoopId = RunLoop::InvalidId;
        _priv::displayMgr displayManager;
        class _priv::renderer renderer;
        GfxResourceContainer resourceContainer;
    };
    static _state* state;
};

//------------------------------------------------------------------------------
template<> inline void
Gfx::ApplyVariable(int32 index, const Id& texResId) {
    o_assert_dbg(IsValid());
    _priv::texture* tex = state->resourceContainer.lookupTexture(texResId);
    state->renderer.applyTexture(index, tex);
}

//------------------------------------------------------------------------------
template<class T> inline void
Gfx::ApplyVariable(int32 index, const T& value) {
    o_assert_dbg(IsValid());
    state->renderer.applyVariable(index, value);
}

//------------------------------------------------------------------------------
template<class T> inline void
Gfx::ApplyVariableArray(int32 index, const T* values, int32 numValues) {
    o_assert_dbg(IsValid());
    state->renderer.applyVariableArray(index, values, numValues);
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SETUP& setup) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.Create(setup);
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SETUP& setup, const Ptr<Stream>& stream) {
    o_assert_dbg(IsValid());
    stream->Open(OpenMode::ReadOnly);
    const void* data = stream->MapRead(nullptr);
    const int32 size = stream->Size();
    Id id = state->resourceContainer.Create(setup, data, size);
    stream->UnmapRead();
    stream->Close();
    return id;
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SetupAndStream<SETUP>& setupAndStream) {
    o_assert_dbg(IsValid());
    return CreateResource(setupAndStream.Setup, setupAndStream.Stream);
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SETUP& setup, const void* data, int32 size) {
    o_assert_dbg(IsValid());
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);
    return state->resourceContainer.Create(setup, data, size);
}

} // namespace Oryol
