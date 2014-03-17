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
#include "Render/Types/TransformType.h"
#include "Render/Types/State.h"
#include "Render/Core/PrimitiveGroup.h"
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
    void Setup(const DisplaySetup& displaySetup);
    /// discard the RenderFacade, tear down rendering system
    void Discard();
    /// return true if the RenderFacade is valid
    bool IsValid() const;
    /// modify the display (may not be supported on all platforms)
    void ModifyDisplay(const DisplaySetup& displaySetup);
    /// test if the window system wants the application to quit
    bool QuitRequested() const;
    
    /// get the original display set object
    const DisplaySetup& GetDisplaySetup() const;
    /// get the current actual display attributes (can be different from setup)
    const DisplayAttrs& GetDisplayAttrs() const;
    
    /// attach a display event handler
    void AttachEventHandler(const Core::Ptr<Messaging::Port>& handler);
    /// detach a display event handler
    void DetachEventHandler(const Core::Ptr<Messaging::Port>& handler);
    
    /// create a resource
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup);
    /// create a resource with data stream
    template<class SETUP> Resource::Id CreateResource(const SETUP& setup, const Core::Ptr<IO::Stream>& data);
    /// discard a resource
    void DiscardResource(const Resource::Id& resId) const;
    /// lookup a resource by resource locator
    Resource::Id LookupResource(const Resource::Locator& locator) const;
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
    displayMgr displayManager;
};
    
} // namespace Render
} // namespace Oryol