//------------------------------------------------------------------------------
//  displayMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "displayMgrBase.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
displayMgrBase::~displayMgrBase() {
    o_assert(!this->displayValid);
}

//------------------------------------------------------------------------------
/**
 The SetupDisplay() method is expected to initialize everything necessary
 for rendering (e.g. creating the application window, create a GL context,...).
 It takes a DisplaySetup object which contains setup parameters like
 width, height, pixelformat, window title, etc...
 This method must be overwritten in a platform-specific subclass.
*/
void
displayMgrBase::SetupDisplay(const GfxDesc& desc) {
    o_assert(!this->displayValid);
    this->displayValid = true;
    this->gfxDesc = desc;
    this->displayAttrs.Width = desc.width;
    this->displayAttrs.Height = desc.height;
    this->displayAttrs.ColorFormat = desc.colorFormat;
    this->displayAttrs.DepthFormat = desc.depthFormat;
    this->displayAttrs.SampleCount = desc.sampleCount;
    this->displayAttrs.Windowed = desc.windowed;
    this->displayAttrs.SwapInterval = desc.swapInterval;
    this->curFramebufferWidth = this->displayAttrs.Width;
    this->curFramebufferHeight = this->displayAttrs.Height;
}

//------------------------------------------------------------------------------
void
displayMgrBase::DiscardDisplay() {
    o_assert(this->displayValid);
    this->displayValid = false;
}

//------------------------------------------------------------------------------
bool
displayMgrBase::IsDisplayValid() const {
    return this->displayValid;
}

//------------------------------------------------------------------------------
/**
 This method is expected to process the platform specific window system 
 messages. This is also usually the place where input events from the
 host system are consumed and forwarded to the Oryol input system. The method
 is usually called at the start of a new frame, and must be overwritten 
 by platform-specific subclasses.
*/
void
displayMgrBase::ProcessSystemEvents() {
    if ((this->curFramebufferWidth != this->displayAttrs.Width) ||
        (this->curFramebufferHeight != this->displayAttrs.Height)) {

        this->curFramebufferWidth = this->displayAttrs.Width;
        this->curFramebufferHeight = this->displayAttrs.Height;
        this->notifyEventHandlers(GfxEvent(GfxEvent::DisplayModified, this->displayAttrs));
    }
}

//------------------------------------------------------------------------------
/**
 This method will be called when all rendering commands for the current
 frames has been issued and the result should be presented. Must be
 overwritten in platform-specific subclass.
*/
void
displayMgrBase::Present() {
    // empty
}

//------------------------------------------------------------------------------
bool
displayMgrBase::QuitRequested() const {
    return false;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
displayMgrBase::GetDisplayAttrs() const {
    return this->displayAttrs;
}

//------------------------------------------------------------------------------
displayMgrBase::eventHandlerId
displayMgrBase::Subscribe(eventHandler handler) {
    eventHandlerId id = this->uniqueIdCounter++;
    this->handlers.Add(id, handler);
    return id;
}

//------------------------------------------------------------------------------
void
displayMgrBase::Unsubscribe(eventHandlerId id) {
    if (this->handlers.Contains(id)) {
        this->handlers.Erase(id);
    }
}

//------------------------------------------------------------------------------
void
displayMgrBase::notifyEventHandlers(const GfxEvent& gfxEvent) {
    for (const auto& handler : this->handlers) {
        handler.Value()(gfxEvent);
    }
}

} // namespace _priv
} // namespace Oryol
