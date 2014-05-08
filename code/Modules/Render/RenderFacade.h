#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::RenderFacade
    @brief public facade of the Render module
    
    @todo: describe RenderFacade
*/
#include "Core/Macros.h"
#include "Render/Core/displayMgr.h"
#include "IO/Stream.h"
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
        
    /// create a resource, or return existing resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup);
    /// create a resource with data stream, or return existing resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup, const Core::Ptr<IO::Stream>& data);
    /// lookup a resource by resource locator (increments use-count of resource!)
    Resource::Id LookupResource(const Resource::Locator& locator);
    /// discard a resource (decrement use-count, free resource if use-count is 0)
    void DiscardResource(const Resource::Id& resId);
    /// get the loading state of a resource
    Resource::State::Code QueryResourceState(const Resource::Id& resId);

    /// begin frame rendering
    bool BeginFrame();
    /// end frame rendering
    void EndFrame();

    /// apply a render target to use
    void ApplyRenderTarget(const Resource::Id& resId);
    /// apply mesh to use for rendering
    void ApplyMesh(const Resource::Id& resId);
    /// apply program to use for rendering
    void ApplyProgram(const Resource::Id& resId, uint32 selectionMask=0);
    /// apply a render state block
    void ApplyStateBlock(const Resource::Id& resId);
    /// apply a shader constant block
    void ApplyConstantBlock(const Resource::Id& resId);
    /// apply state with 1..4 arguments
    template<typename... ARGS> void ApplyState(State::Code state, ARGS... args);
    /// apply a shader variable
    template<class T> void ApplyVariable(int32 index, const T& value);
    /// apply a shader variable array
    template<class T> void ApplyVariableArray(int32 index, const T* values, int32 numValues);
    
    /// clear the currently assigned render target
    void Clear(bool color, bool depth, bool stencil);
    /// submit a draw call with primitive group index in current mesh
    void Draw(int32 primGroupIndex);
    /// submit a draw call with overridden primitive group
    void Draw(const PrimitiveGroup& primGroup);
    /// draw multiple instances
    void Draw(int32 primGroupIndex, const glm::mat4* instanceTransforms, int32 numInstances);
    /// draw multiple instances with primitive group override
    void Draw(const PrimitiveGroup& primGroup, const glm::mat4* instanceTransforms, int32 numInstances);
    /// draw a fullscreen quad
    void DrawFullscreenQuad();

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
RenderFacade::ApplyState(State::Code state, bool b) {
    this->stateWrapper.ApplyState(state, b);
}

//------------------------------------------------------------------------------
template<> inline void
RenderFacade::ApplyState(State::Code state, State::Value val) {
    this->stateWrapper.ApplyState(state, val);
}

//------------------------------------------------------------------------------
template<> inline void
RenderFacade::ApplyState(State::Code state, float32 v0) {
    this->stateWrapper.ApplyState(state, v0);
}

//------------------------------------------------------------------------------
template<> inline void
RenderFacade::ApplyState(State::Code state, float32 v0, float32 v1, float32 v2, float32 v3) {
    this->stateWrapper.ApplyState(state, v0, v1, v2, v3);
}

//------------------------------------------------------------------------------
template<class T> inline void
RenderFacade::AttachLoader(Core::Ptr<T> loader) {
    this->resourceManager.AttachLoader(loader.get());
}

} // namespace Render
} // namespace Oryol
