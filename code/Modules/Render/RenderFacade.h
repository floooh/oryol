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
#include "glm/fwd.hpp"

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
    
    /// attach a resource loader
    template<class T> void AttachLoader(Core::Ptr<T> loader);
    /// attach a display event handler
    void AttachEventHandler(const Core::Ptr<Messaging::Port>& handler);
    /// detach a display event handler
    void DetachEventHandler(const Core::Ptr<Messaging::Port>& handler);
    
    /// get the original render setup object
    const RenderSetup& GetRenderSetup() const;
    /// get the current actual display attributes (can be different from setup)
    const DisplayAttrs& GetDisplayAttrs() const;
    /// test if an optional feature is supported
    bool Supports(Feature::Code feat) const;
        
    /// create a resource, or return existing resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup);
    /// create a resource with data stream, or return existing resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup, const Core::Ptr<IO::Stream>& data);
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

    /// make the default render target (backbuffer) current
    void ApplyDefaultRenderTarget();
    /// apply an offscreen render target
    void ApplyOffscreenRenderTarget(const Resource::Id& resId);
    /// apply draw state to use for rendering
    void ApplyDrawState(const Resource::Id& resId);
    /// apply a shader constant block
    void ApplyConstantBlock(const Resource::Id& resId);
    /// apply state with 1..4 arguments
    template<typename... ARGS> void ApplyState(State::Code state, ARGS... args);
    /// apply a shader variable
    template<class T> void ApplyVariable(int32 index, const T& value);
    /// apply a shader variable array
    template<class T> void ApplyVariableArray(int32 index, const T* values, int32 numValues);
    
    /// update dynamic vertex data (only complete replace possible at the moment)
    void UpdateVertices(const Resource::Id& resId, int32 numBytes, const void* data);
    /// update dynamic index data (only complete replace possible at the moment)
    void UpdateIndices(const Resource::Id& resId, int32 numBytes, const void* data);
    
    /// clear the currently assigned render target
    void Clear(bool color, bool depth, bool stencil);
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
RenderFacade::CreateResource(const SETUP& setup, const Core::Ptr<IO::Stream>& data) {
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
template<> inline void
RenderFacade::ApplyState(State::Code state, float32 f0) {
    this->stateWrapper.ApplyState(state, f0);
}

//------------------------------------------------------------------------------
template<> inline void
RenderFacade::ApplyState(State::Code state, float32 f0, float32 f1) {
    this->stateWrapper.ApplyState(state, f0, f1);
}

//------------------------------------------------------------------------------
template<> inline void
RenderFacade::ApplyState(State::Code state, float32 f0, float32 f1, float32 f2, float32 f3) {
    this->stateWrapper.ApplyState(state, f0, f1, f2, f3);
}

//------------------------------------------------------------------------------
template<> inline void
RenderFacade::ApplyState(State::Code state, int32 i0) {
    this->stateWrapper.ApplyState(state, i0);
}
    
//------------------------------------------------------------------------------
template<> inline void
RenderFacade::ApplyState(State::Code state, int32 i0, int32 i1, int32 i2, int32 i3) {
    this->stateWrapper.ApplyState(state, i0, i1, i2, i3);
}

//------------------------------------------------------------------------------
template<class T> inline void
RenderFacade::AttachLoader(Core::Ptr<T> loader) {
    this->resourceManager.AttachLoader(loader.get());
}

//------------------------------------------------------------------------------
inline bool
RenderFacade::Supports(Feature::Code feat) const {
    return this->renderManager.Supports(feat);
}

} // namespace Render
} // namespace Oryol
