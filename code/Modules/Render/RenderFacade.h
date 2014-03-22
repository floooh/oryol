#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::RenderFacade
    @brief public facade of the Render module
    
    @todo: describe RenderFacade
*/
#include "Core/Macros.h"
#include "Render/Core/displayMgr.h"
#include "Render/Core/transformMgr.h"
#include "IO/Stream.h"
#include "Resource/Id.h"
#include "Resource/State.h"
#include "Resource/Locator.h"
#include "Render/Setup/RenderSetup.h"
#include "Render/Types/TransformType.h"
#include "Render/Types/State.h"
#include "Render/Core/PrimitiveGroup.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/resourceMgr.h"
#include "Render/Setup/MeshSetup.h"
#include "glm/fwd.hpp"

namespace Oryol {
namespace Render {
    
class RenderFacade {
    OryolLocalSingletonDecl(RenderFacade);
public:
    /// constructor
    RenderFacade();
    /// destructor
    ~RenderFacade();
    
    /// setup the RenderFacade, initialize rendering system
    void Setup(const RenderSetup& renderSetup);
    /// discard the RenderFacade, tear down rendering system
    void Discard();
    /// return true if the RenderFacade is valid
    bool IsValid() const;
    /// modify the display (may not be supported on all platforms)
    void ModifyDisplay(const RenderSetup& displaySetup);
    /// test if the window system wants the application to quit
    bool QuitRequested() const;
    /// attach a resource loader
    template<class LOADER> void AttachLoader(const Core::Ptr<LOADER>& loader);
    /// attach a display event handler
    void AttachEventHandler(const Core::Ptr<Messaging::Port>& handler);
    /// detach a display event handler
    void DetachEventHandler(const Core::Ptr<Messaging::Port>& handler);
    
    /// get the original render set object
    const RenderSetup& GetRenderSetup() const;
    /// get the current actual display attributes (can be different from setup)
    const DisplayAttrs& GetDisplayAttrs() const;
        
    /// create a resource, or return existing resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup);
    /// create a resource with data stream, or return existing resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup, const Core::Ptr<IO::Stream>& data);
    /// lookup a resource by resource locator (incremenets use-count of resource!)
    Resource::Id LookupResource(const Resource::Locator& locator);
    /// discard a resource (decrement use-count, free resource if use-count is 0)
    void DiscardResource(const Resource::Id& resId);
    /// get the loading state of a resource
    Resource::State::Code QueryResourceState(const Resource::Id& resId) const;

    /// get one of the transform matrices
    const glm::mat4& QueryTransform(TransformType::Code transformType) const;
    
    /// begin frame rendering
    bool BeginFrame();
    /// end frame rendering
    void EndFrame();

    /// apply the model transform
    void ApplyModelTransform(const glm::mat4& model);
    /// apply the view transform
    void ApplyViewTransform(const glm::mat4& view);
    /// apply the projection transform
    void ApplyProjTransform(const glm::mat4& proj);
    /// apply a render target to use
    void ApplyRenderTarget(const Resource::Id& resId);
    /// apply mesh to use for rendering
    void ApplyMesh(const Resource::Id& resId);
    /// apply program to use for rendering
    void ApplyProgram(const Resource::Id& resId, uint32 selector=0);
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
    RenderSetup renderSetup;
    displayMgr displayManager;
    transformMgr transformManager;
    stateWrapper stateWrapper;
    resourceMgr resourceManager;
};

//------------------------------------------------------------------------------
inline void
RenderFacade::ApplyModelTransform(const glm::mat4& m) {
    this->transformManager.SetModel(m);
}

//------------------------------------------------------------------------------
inline void
RenderFacade::ApplyViewTransform(const glm::mat4& m) {
    this->transformManager.SetView(m);
}

//------------------------------------------------------------------------------
inline void
RenderFacade::ApplyProjTransform(const glm::mat4& m) {
    this->transformManager.SetProj(m);
}

//------------------------------------------------------------------------------
inline const glm::mat4&
RenderFacade::QueryTransform(TransformType::Code type) const {
    return this->transformManager.GetTransform(type);
}

//------------------------------------------------------------------------------
template<class SETUP> Resource::Id
RenderFacade::CreateResource(const SETUP& setup) {
    o_assert(this->IsValid());
    return this->resourceManager.CreateResource(setup);
}

//------------------------------------------------------------------------------
template<class SETUP> Resource::Id
RenderFacade::CreateResource(const SETUP& setup, const Core::Ptr<IO::Stream>& data) {
    o_assert(this->IsValid());
    return this->resourceManager.CreateResource(setup, data);
}

} // namespace Render
} // namespace Oryol