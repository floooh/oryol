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
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Core/resourceMgr.h"
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
        
    /// create a gfx resource
    template<class SETUP> static Id CreateResource(const SETUP& setup);
    /// create a gfx resource with data in stream object
    template<class SETUP> static Id CreateResource(const SETUP& setup, const Ptr<Stream>& data);
    /// allocate a resource (for async resource loading)
    template<class SETUP> static Id AllocResource(const SETUP& setup, Id placeholder=Id::InvalidId());
    /// setup a previously allocated resource from data (for async resource loading)
    static void SetupResource(const Id& id, const Ptr<Stream>& data);
    /// discard a gfx resource
    static void DiscardResource(const Id& id);
    /// get the loading state of a resource
    static ResourceState::Code QueryResourceState(const Id& id);
    /// query the resource setup object for an id
    template<class SETUP> const SETUP& QueryResourceSetup(const Id& id) const;
    
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
    static void UpdateVertices(const Id& id, int32 numBytes, const void* data);
    /// update dynamic index data (only complete replace possible at the moment)
    static void UpdateIndices(const Id& id, int32 numBytes, const void* data);
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
    struct _state {
        class GfxSetup gfxSetup;
        RunLoop::Id runLoopId = RunLoop::InvalidId;
        _priv::displayMgr displayManager;
        class _priv::renderer renderer;
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
template<class SETUP> inline Id
Gfx::CreateResource(const SETUP& setup) {
    o_assert_dbg(IsValid());
    return state->resourceManager.CreateResource(setup);
}

//------------------------------------------------------------------------------
template<class SETUP> inline Id
Gfx::CreateResource(const SETUP& setup, const Ptr<Stream>& data) {
    o_assert_dbg(IsValid());
    return state->resourceManager.CreateResource(setup, data);
}

//------------------------------------------------------------------------------
inline void
Gfx::DiscardResource(const Id& id) {
    o_assert_dbg(IsValid());
    state->resourceManager.DiscardResource(id);
}

//------------------------------------------------------------------------------
template<> inline void
Gfx::ApplyVariable(int32 index, const Id& texResId) {
    o_assert_dbg(IsValid());
    _priv::texture* tex = state->resourceManager.LookupTexture(texResId.Id());
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
inline bool
Gfx::Supports(GfxFeature::Code feat) {
    o_assert_dbg(IsValid());
    return state->renderer.supports(feat);
}

//------------------------------------------------------------------------------
inline void
Gfx::ApplyViewPort(int32 x, int32 y, int32 width, int32 height) {
    o_assert_dbg(IsValid());
    state->renderer.applyViewPort(x, y, width, height);
}

//------------------------------------------------------------------------------
inline void
Gfx::ApplyScissorRect(int32 x, int32 y, int32 width, int32 height) {
    o_assert_dbg(IsValid());
    state->renderer.applyScissorRect(x, y, width, height);
}

//------------------------------------------------------------------------------
inline void
Gfx::ApplyBlendColor(const glm::vec4& blendColor) {
    o_assert_dbg(IsValid());
    state->renderer.applyBlendColor(blendColor);
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
