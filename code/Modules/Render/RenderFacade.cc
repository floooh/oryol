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
RenderFacade::RenderFacade() :
isValid(false) {
    this->SingletonEnsureUnique();
}

//------------------------------------------------------------------------------
RenderFacade::~RenderFacade() {
    if (this->isValid) {
        this->Discard();
    }
}

//------------------------------------------------------------------------------
void
RenderFacade::Setup(const RenderSetup& setup) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->renderSetup = setup;
    this->displayManager.SetupDisplay(setup);
    this->stateWrapper.Setup();
    this->resourceManager.Setup(setup, &this->stateWrapper);
    this->renderManager.Setup(&this->stateWrapper);
}

//------------------------------------------------------------------------------
void
RenderFacade::Discard() {
    o_assert_dbg(this->isValid);
    this->renderManager.Discard();
    this->resourceManager.Discard();
    this->stateWrapper.Discard();
    this->displayManager.DiscardDisplay();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
RenderFacade::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
RenderFacade::ModifyDisplay(const RenderSetup& renderSetup) {
    o_assert_dbg(this->isValid);
    this->displayManager.ModifyDisplay(renderSetup);
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
    o_assert_dbg(this->isValid);
    return this->resourceManager.LookupResource(loc);
}

//------------------------------------------------------------------------------
void
RenderFacade::DiscardResource(const Id& resId) {
    o_assert_dbg(this->isValid);
    this->resourceManager.DiscardResource(resId);
}

//------------------------------------------------------------------------------
Resource::State::Code
RenderFacade::QueryResourceState(const Id& resId) {
    o_assert_dbg(this->isValid);
    return this->resourceManager.QueryResourceState(resId);
}

//------------------------------------------------------------------------------
void
RenderFacade::ApplyMesh(const Id& resId) {
    o_assert_dbg(this->isValid);
    this->renderManager.ApplyMesh(this->resourceManager.LookupMesh(resId));
}

//------------------------------------------------------------------------------
void
RenderFacade::ApplyProgram(const Id& resId, uint32 selMask) {
    o_assert_dbg(this->isValid);
    this->renderManager.ApplyProgram(this->resourceManager.LookupProgramBundle(resId), selMask);
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
RenderFacade::Clear(bool color, bool depth, bool stencil) {
    o_assert_dbg(this->isValid);
    this->renderManager.Clear(color, depth, stencil);
}

//------------------------------------------------------------------------------
void
RenderFacade::Draw(int32 primGroupIndex) {
    o_assert_dbg(this->isValid);
    this->renderManager.Draw(primGroupIndex);
}

//------------------------------------------------------------------------------
void
RenderFacade::Draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(this->isValid);
    this->renderManager.Draw(primGroup);
}

} // namespace Render
} // namespace Oryol