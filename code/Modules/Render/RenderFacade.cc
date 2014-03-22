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
RenderFacade::RenderFacade() {
    this->SingletonEnsureUnique();
}

//------------------------------------------------------------------------------
RenderFacade::~RenderFacade() {
    if (this->IsValid()) {
        this->Discard();
    }
}

//------------------------------------------------------------------------------
void
RenderFacade::Setup(const RenderSetup& setup) {
    o_assert(!this->IsValid());
    this->renderSetup = setup;
    this->displayManager.SetupDisplay(setup);
    this->stateWrapper.Setup();
    this->resourceManager.Setup(setup, &this->stateWrapper);
}

//------------------------------------------------------------------------------
void
RenderFacade::Discard() {
    o_assert(this->IsValid());
    this->resourceManager.Discard();
    this->stateWrapper.Discard();
    this->displayManager.DiscardDisplay();
}

//------------------------------------------------------------------------------
bool
RenderFacade::IsValid() const {
    return this->displayManager.IsDisplayValid();
}

//------------------------------------------------------------------------------
void
RenderFacade::ModifyDisplay(const RenderSetup& renderSetup) {
    o_assert(this->IsValid());
    this->displayManager.ModifyDisplay(renderSetup);
}

//------------------------------------------------------------------------------
bool
RenderFacade::QuitRequested() const {
    return this->displayManager.QuitRequested();
}

//------------------------------------------------------------------------------
template<class LOADER> void
RenderFacade::AttachLoader(const Ptr<LOADER>& loader) {
    this->resourceManager.AttachLoader(loader);
}

//------------------------------------------------------------------------------
void
RenderFacade::AttachEventHandler(const Ptr<Port>& handler) {
    o_assert(handler.isValid());
    this->displayManager.AttachDisplayEventHandler(handler);
}

//------------------------------------------------------------------------------
void
RenderFacade::DetachEventHandler(const Ptr<Port>& handler) {
    o_assert(handler.isValid());
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
    o_assert(this->IsValid());
    return this->resourceManager.LookupResource(loc);
}

//------------------------------------------------------------------------------
void
RenderFacade::DiscardResource(const Id& resId) {
    o_assert(this->IsValid());
    this->resourceManager.DiscardResource(resId);
}

//------------------------------------------------------------------------------
Resource::State::Code
RenderFacade::QueryResourceState(const Id& resId) const {
    o_assert(this->IsValid());
    return this->resourceManager.QueryResourceState(resId);
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

} // namespace Render
} // namespace Oryol