#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::RenderFacade
    @brief public facade of the Render module
    
    @todo: describe RenderFacade
*/
#include "Core/Singleton.h"
#include "Render/Core/displayMgr.h"
#include "IO/Stream/Stream.h"
#include "Resource/Id.h"
#include "Resource/State.h"
#include "Resource/Locator.h"
#include "Render/Setup/RenderSetup.h"
#include "Render/Core/Enums.h"
#include "Render/Core/PrimitiveGroup.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/resourceMgr.h"
#include "Render/Core/renderMgr.h"
#include "Render/Setup/MeshSetup.h"
#include "glm/vec4.hpp"

namespace Oryol {
namespace Render {
    
class RenderFacade {
    OryolLocalSingletonDecl(RenderFacade);
public:
    /// constructor
    RenderFacade(const RenderSetup& renderSetup);
    /// destructor
    ~RenderFacade();
    
    /// test if the window system wants the application to quit
    bool QuitRequested() const;
    
    /// attach a display event handler
    void AttachEventHandler(const Ptr<Messaging::Port>& handler);
    /// detach a display event handler
    void DetachEventHandler(const Ptr<Messaging::Port>& handler);
    
    /// get the original render setup object
    const RenderSetup& GetRenderSetup() const;
    /// get the current actual display attributes (can be different from setup)
    const DisplayAttrs& GetDisplayAttrs() const;
    /// test if an optional feature is supported
    bool Supports(Feature::Code feat) const;
        
    /// create a resource, or return existing resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup);
    /// create a resource with data stream, or return existing resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup, const Ptr<Stream>& data);
    /// lookup a resource by resource locator (increments use-count of resource!)
    Resource::Id LookupResource(const Resource::Locator& locator);
    /// release a resource (decrement use-count, free resource if use-count is 0)
    void ReleaseResource(const Resource::Id& resId);
    /// get the loading state of a resource
    Resource::State::Code QueryResourceState(const Resource::Id& resId);
    
    /// begin frame rendering
    bool BeginFrame();
    /// end frame rendering
    void EndFrame();
    /// reset internal state (must be called when directly rendering through GL; FIXME: better name?)
    void ResetStateCache();

    /// make the default render target (backbuffer) current
    void ApplyDefaultRenderTarget();
    /// apply an offscreen render target
    void ApplyOffscreenRenderTarget(const Resource::Id& resId);
    /// apply view port
    void ApplyViewPort(int32 x, int32 y, int32 width, int32 height);
    /// apply scissor rect (must also be enabled in DrawState.RasterizerState)
    void ApplyScissorRect(int32 x, int32 y, int32 width, int32 height);
    /// apply blend color (see DrawState.BlendState)
    void ApplyBlendColor(const glm::vec4& blendColor);
    /// apply draw state to use for rendering
    void ApplyDrawState(const Resource::Id& resId);
    /// apply a shader constant block
    void ApplyConstantBlock(const Resource::Id& resId);
    /// apply a shader variable
    template<class T> void ApplyVariable(int32 index, const T& value);
    /// apply a shader variable array
    template<class T> void ApplyVariableArray(int32 index, const T* values, int32 numValues);
    
    /// update dynamic vertex data (only complete replace possible at the moment)
    void UpdateVertices(const Resource::Id& resId, int32 numBytes, const void* data);
    /// update dynamic index data (only complete replace possible at the moment)
    void UpdateIndices(const Resource::Id& resId, int32 numBytes, const void* data);
    /// read current framebuffer pixels into client memory, this means a PIPELINE STALL!!
    void ReadPixels(void* ptr, int32 numBytes);
    
    /// clear the currently assigned render target
    void Clear(Channel::Mask channels, const glm::vec4& color, float32 depth, uint8 stencil);
    /// submit a draw call with primitive group index in current mesh
    void Draw(int32 primGroupIndex);
    /// submit a draw call with direct primitive group
    void Draw(const PrimitiveGroup& primGroup);
    /// submit a draw call for instanced rendering
    void DrawInstanced(int32 primGroupIndex, int32 numInstances);
    /// submit a draw call for instanced rendering with direct primitive group
    void DrawInstanced(const PrimitiveGroup& primGroup, int32 numInstances);

private:
    /// setup the RenderFacade, initialize rendering system
    void setup(const RenderSetup& renderSetup);
    /// discard the RenderFacade, tear down rendering system
    void discard();
    /// return true if the RenderFacade is valid
    bool isValid() const;

    bool valid;
    RenderSetup renderSetup;
    displayMgr displayManager;
    renderMgr renderManager;
    class stateWrapper stateWrapper;
    resourceMgr resourceManager;
};

//------------------------------------------------------------------------------
template<class SETUP> inline Resource::Id
RenderFacade::CreateResource(const SETUP& setup) {
    o_assert_dbg(this->valid);
    return this->resourceManager.CreateResource(setup);
}

//------------------------------------------------------------------------------
template<class SETUP> inline Resource::Id
RenderFacade::CreateResource(const SETUP& setup, const Ptr<Stream>& data) {
    o_assert_dbg(this->valid);
    return this->resourceManager.CreateResource(setup, data);
}

//------------------------------------------------------------------------------
template<> inline void
RenderFacade::ApplyVariable(int32 index, const Resource::Id& texResId) {
    o_assert_dbg(this->valid);
    texture* tex = this->resourceManager.LookupTexture(texResId);
    this->renderManager.ApplyTexture(index, tex);
}

//------------------------------------------------------------------------------
template<class T> inline void
RenderFacade::ApplyVariable(int32 index, const T& value) {
    o_assert_dbg(this->valid);
    this->renderManager.ApplyVariable(index, value);
}

//------------------------------------------------------------------------------
template<class T> inline void
RenderFacade::ApplyVariableArray(int32 index, const T* values, int32 numValues) {
    o_assert_dbg(this->valid);
    this->renderManager.ApplyVariableArray(index, values, numValues);
}

//------------------------------------------------------------------------------
inline bool
RenderFacade::Supports(Feature::Code feat) const {
    return this->renderManager.Supports(feat);
}

//------------------------------------------------------------------------------
inline void
RenderFacade::ApplyViewPort(int32 x, int32 y, int32 width, int32 height) {
    this->stateWrapper.ApplyViewPort(x, y, width, height);
}

//------------------------------------------------------------------------------
inline void
RenderFacade::ApplyScissorRect(int32 x, int32 y, int32 width, int32 height) {
    this->stateWrapper.ApplyScissorRect(x, y, width, height);
}

//------------------------------------------------------------------------------
inline void
RenderFacade::ApplyBlendColor(const glm::vec4& blendColor) {
    this->stateWrapper.ApplyBlendColor(blendColor.x, blendColor.y, blendColor.z, blendColor.w);
}

} // namespace Render
} // namespace Oryol
