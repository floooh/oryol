//------------------------------------------------------------------------------
//  RenderFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RenderFacade.h"

namespace Oryol {
namespace Render {

OryolLocalSingletonImpl(RenderFacade);

using namespace Core;
using namespace IO;
using namespace Resource;
using namespace Messaging;
    
//------------------------------------------------------------------------------
RenderFacade::RenderFacade(const RenderSetup& setup) :
valid(false) {
    this->SingletonEnsureUnique();
    this->setup(setup);
}

//------------------------------------------------------------------------------
RenderFacade::~RenderFacade() {
    if (this->valid) {
        this->discard();
    }
}

//------------------------------------------------------------------------------
void
RenderFacade::setup(const RenderSetup& setup) {
    o_assert_dbg(!this->valid);
    this->valid = true;
    this->renderSetup = setup;
    this->displayManager.SetupDisplay(setup);
    this->stateWrapper.Setup();
    this->resourceManager.Setup(setup, &this->stateWrapper, &this->displayManager);
    this->renderManager.Setup(&this->stateWrapper, &this->displayManager);
    this->resourceManager.createFullscreenQuadMesh(this->fullscreenQuadMesh);
}

//------------------------------------------------------------------------------
void
RenderFacade::discard() {
    o_assert_dbg(this->valid);
    this->resourceManager.discardFullscreenQuadMesh(this->fullscreenQuadMesh);
    this->renderManager.Discard();
    this->resourceManager.Discard();
    this->stateWrapper.Discard();
    this->displayManager.DiscardDisplay();
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
RenderFacade::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
bool
RenderFacade::QuitRequested() const {
    return this->displayManager.QuitRequested();
}

//------------------------------------------------------------------------------
void
RenderFacade::AttachEventHandler(const Ptr<Port>& handler) {
    o_assert_dbg(handler.isValid());
    this->displayManager.AttachDisplayEventHandler(handler);
}

//------------------------------------------------------------------------------
void
RenderFacade::DetachEventHandler(const Ptr<Port>& handler) {
    o_assert_dbg(handler.isValid());
    this->displayManager.DetachDisplayEventHandler(handler);
}

//------------------------------------------------------------------------------
const RenderSetup&
RenderFacade::GetRenderSetup() const {
    return this->renderSetup;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
RenderFacade::GetDisplayAttrs() const {
    return this->displayManager.GetDisplayAttrs();
}

//------------------------------------------------------------------------------
/**
 NOTE: the LookupResource() method will bump the use-count of looked up
 resource, so make sure to call DiscardResource() when you're done with it!
*/
Id
RenderFacade::LookupResource(const Locator& loc) {
    o_assert_dbg(this->valid);
    return this->resourceManager.LookupResource(loc);
}

//------------------------------------------------------------------------------
void
RenderFacade::DiscardResource(const Id& resId) {
    o_assert_dbg(this->valid);
    this->resourceManager.DiscardResource(resId);
}

//------------------------------------------------------------------------------
Resource::State::Code
RenderFacade::QueryResourceState(const Id& resId) {
    o_assert_dbg(this->valid);
    return this->resourceManager.QueryResourceState(resId);
}

//------------------------------------------------------------------------------
void
RenderFacade::ApplyRenderTarget(const Id& resId) {
    o_assert_dbg(this->valid);
    int32 width, height;
    if (!resId.IsValid()) {
        // apply default framebuffer
        this->renderManager.ApplyRenderTarget(nullptr);
        
        // update viewport
        const DisplayAttrs& attrs = this->displayManager.GetDisplayAttrs();
        width = attrs.GetFramebufferWidth();
        height = attrs.GetFramebufferHeight();
    }
    else {
        texture* renderTarget = this->resourceManager.LookupTexture(resId);
        o_assert_dbg(nullptr != renderTarget);
        this->renderManager.ApplyRenderTarget(renderTarget);
        
        // update viewport
        const TextureAttrs& attrs = renderTarget->GetTextureAttrs();
        width = attrs.GetWidth();
        height = attrs.GetHeight();
    }
    // update viewport to cover full render target
    this->stateWrapper.ApplyState(State::ViewPort, 0, 0, width, height);
}

//------------------------------------------------------------------------------
void
RenderFacade::ApplyMesh(const Id& resId) {
    o_assert_dbg(this->valid);
    this->renderManager.ApplyMesh(this->resourceManager.LookupMesh(resId));
}

//------------------------------------------------------------------------------
void
RenderFacade::ApplyProgram(const Id& resId, uint32 selMask) {
    o_assert_dbg(this->valid);
    this->renderManager.ApplyProgram(this->resourceManager.LookupProgramBundle(resId), selMask);
}

//------------------------------------------------------------------------------
void
RenderFacade::ApplyDepthStencilState(const Id& resId) {
    o_assert_dbg(this->valid);
    this->stateWrapper.ApplyDepthStencilState(this->resourceManager.LookupDepthStencilState(resId));
}

//------------------------------------------------------------------------------
void
RenderFacade::ApplyStateBlock(const Id& resId) {
    o_assert_dbg(this->valid);
    this->stateWrapper.ApplyStateBlock(this->resourceManager.LookupStateBlock(resId));
}

//------------------------------------------------------------------------------
bool
RenderFacade::BeginFrame() {
    this->resourceManager.Update();
    this->displayManager.ProcessSystemEvents();
    
    /// @todo: check and return whether rendering is possible / necessary
    return true;
}

//------------------------------------------------------------------------------
void
RenderFacade::EndFrame() {
    this->displayManager.Present();
}

//------------------------------------------------------------------------------
void
RenderFacade::UpdateVertices(const Resource::Id& resId, int32 numBytes, const void* data) {
    o_assert_dbg(this->valid);
    mesh* msh = this->resourceManager.LookupMesh(resId);
    this->renderManager.UpdateVertices(msh, numBytes, data);
}

//------------------------------------------------------------------------------
void
RenderFacade::Clear(bool color, bool depth, bool stencil) {
    o_assert_dbg(this->valid);
    this->renderManager.Clear(color, depth, stencil);
}

//------------------------------------------------------------------------------
void
RenderFacade::Draw(int32 primGroupIndex) {
    o_assert_dbg(this->valid);
    this->renderManager.Draw(primGroupIndex);
}

//------------------------------------------------------------------------------
void
RenderFacade::Draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(this->valid);
    this->renderManager.Draw(primGroup);
}

//------------------------------------------------------------------------------
void
RenderFacade::DrawFullscreenQuad() {
    o_assert_dbg(this->valid);
    this->renderManager.ApplyMesh(&this->fullscreenQuadMesh);
    this->renderManager.Draw(0);
}

} // namespace Render
} // namespace Oryol
