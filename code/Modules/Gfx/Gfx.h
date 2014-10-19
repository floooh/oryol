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
#include "Resource/Id.h"
#include "Resource/ResourceState.h"
#include "Resource/Locator.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Core/stateWrapper.h"
#include "Gfx/Core/resourceMgr.h"
#include "Gfx/Core/renderMgr.h"
#include "Gfx/Core/GfxId.h"
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
        
    /// create a resource, or return existing shared resource
    template<class SETUP> static GfxId CreateResource(const SETUP& setup);
    /// create a resource with data stream, or return existing shared resource
    template<class SETUP> static GfxId CreateResource(const SETUP& setup, const Ptr<Stream>& data);
    /// lookup a resource by resource locator
    static GfxId LookupResource(const Locator& locator);
    /// get the loading state of a resource
    static ResourceState::Code QueryResourceState(const GfxId& gfxId);
    
    /// make the default render target (backbuffer) current
    static void ApplyDefaultRenderTarget();
    /// apply an offscreen render target
    static void ApplyOffscreenRenderTarget(const GfxId& gfxId);
    /// apply view port
    static void ApplyViewPort(int32 x, int32 y, int32 width, int32 height);
    /// apply scissor rect (must also be enabled in DrawState.RasterizerState)
    static void ApplyScissorRect(int32 x, int32 y, int32 width, int32 height);
    /// apply blend color (see DrawState.BlendState)
    static void ApplyBlendColor(const glm::vec4& blendColor);
    /// apply draw state to use for rendering
    static void ApplyDrawState(const GfxId& gfxId);
    /// apply a shader constant block
    static void ApplyConstantBlock(const GfxId& gfxId);
    /// apply a shader variable
    template<class T> static void ApplyVariable(int32 index, const T& value);
    /// apply a shader variable array
    template<class T> static void ApplyVariableArray(int32 index, const T* values, int32 numValues);
    
    /// update dynamic vertex data (only complete replace possible at the moment)
    static void UpdateVertices(const GfxId& gfxId, int32 numBytes, const void* data);
    /// update dynamic index data (only complete replace possible at the moment)
    static void UpdateIndices(const GfxId& gfxId, int32 numBytes, const void* data);
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
    
private:
    friend class GfxId;
    
    /// increment resource id use count
    static void useResource(const Id& id);
    /// decrement resource id use count
    static void releaseResource(const Id& id);

    struct _state {
        class GfxSetup gfxSetup;
        RunLoop::Id runLoopId = RunLoop::InvalidId;
        _priv::displayMgr displayManager;
        _priv::renderMgr renderManager;
        _priv::stateWrapper stateWrapper;
        _priv::resourceMgr resourceManager;
    };
    static _state* state;
};

//------------------------------------------------------------------------------
inline bool
Gfx::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
template<class SETUP> inline GfxId
Gfx::CreateResource(const SETUP& setup) {
    o_assert_dbg(IsValid());
    return GfxId(state->resourceManager.CreateResource(setup));
}

//------------------------------------------------------------------------------
template<class SETUP> inline GfxId
Gfx::CreateResource(const SETUP& setup, const Ptr<Stream>& data) {
    o_assert_dbg(IsValid());
    return GfxId(state->resourceManager.CreateResource(setup, data));
}

//------------------------------------------------------------------------------
template<> inline void
Gfx::ApplyVariable(int32 index, const GfxId& texResId) {
    o_assert_dbg(IsValid());
    _priv::texture* tex = state->resourceManager.LookupTexture(texResId.Id());
    state->renderManager.ApplyTexture(index, tex);
}

//------------------------------------------------------------------------------
template<class T> inline void
Gfx::ApplyVariable(int32 index, const T& value) {
    o_assert_dbg(IsValid());
    state->renderManager.ApplyVariable(index, value);
}

//------------------------------------------------------------------------------
template<class T> inline void
Gfx::ApplyVariableArray(int32 index, const T* values, int32 numValues) {
    o_assert_dbg(IsValid());
    state->renderManager.ApplyVariableArray(index, values, numValues);
}

//------------------------------------------------------------------------------
inline bool
Gfx::Supports(GfxFeature::Code feat) {
    o_assert_dbg(IsValid());
    return state->renderManager.Supports(feat);
}

//------------------------------------------------------------------------------
inline void
Gfx::ApplyViewPort(int32 x, int32 y, int32 width, int32 height) {
    o_assert_dbg(IsValid());
    state->stateWrapper.ApplyViewPort(x, y, width, height);
}

//------------------------------------------------------------------------------
inline void
Gfx::ApplyScissorRect(int32 x, int32 y, int32 width, int32 height) {
    o_assert_dbg(IsValid());
    state->stateWrapper.ApplyScissorRect(x, y, width, height);
}

//------------------------------------------------------------------------------
inline void
Gfx::ApplyBlendColor(const glm::vec4& blendColor) {
    o_assert_dbg(IsValid());
    state->stateWrapper.ApplyBlendColor(blendColor.x, blendColor.y, blendColor.z, blendColor.w);
}

//------------------------------------------------------------------------------
inline void
Gfx::releaseResource(const Id& resId) {
    if (IsValid()) {
        state->resourceManager.ReleaseResource(resId);
    }
}

//------------------------------------------------------------------------------
inline void
Gfx::useResource(const Id& resId) {
    if (IsValid()) {
        state->resourceManager.UseResource(resId);
    }
}

} // namespace Oryol
