//------------------------------------------------------------------------------
//  RenderFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RenderFacade.h"

namespace Oryol {
namespace Render {
    
OryolLocalSingletonImpl(RenderFacade);
    
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
RenderFacade::Setup(const DisplaySetup& displaySetup) {
    o_assert(!this->IsValid());
    this->displayManager.SetupDisplay(displaySetup);
}

//------------------------------------------------------------------------------
void
RenderFacade::Discard() {
    o_assert(this->IsValid());
    this->displayManager.DiscardDisplay();
}

//------------------------------------------------------------------------------
bool
RenderFacade::IsValid() const {
    return this->displayManager.IsDisplayValid();
}

//------------------------------------------------------------------------------
void
RenderFacade::ModifyDisplay(const DisplaySetup& displaySetup) {
    o_assert(this->IsValid());
    this->displayManager.ModifyDisplay(displaySetup);
}

//------------------------------------------------------------------------------
bool
RenderFacade::QuitRequested() const {
    return this->displayManager.QuitRequested();
}

//------------------------------------------------------------------------------
const DisplaySetup&
RenderFacade::GetDisplaySetup() const {
    return this->displayManager.GetDisplaySetup();
}

//------------------------------------------------------------------------------
const DisplayAttrs&
RenderFacade::GetDisplayAttrs() const {
    return this->displayManager.GetDisplayAttrs();
}

//------------------------------------------------------------------------------
bool
RenderFacade::BeginFrame() {
    this->displayManager.ProcessSystemEvents();
    
    /// @todo: check and return wheter rendering is possible / necessary
    return true;
}

//------------------------------------------------------------------------------
void
RenderFacade::EndFrame() {
    this->displayManager.Present();
}

} // namespace Render
} // namespace Oryol