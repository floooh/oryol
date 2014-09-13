#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::displayMgrBase
    @ingroup _priv
    @brief private: display managers base class
    
    @see displayMgr
*/
#include "Render/Setup/RenderSetup.h"
#include "Render/Attrs/DisplayAttrs.h"
#include "Core/Containers/Array.h"
#include "Messaging/Port.h"

namespace Oryol {
namespace _priv {
    
class displayMgrBase {
public:
    /// constructor
    displayMgrBase();
    /// destructor
    ~displayMgrBase();
    
    /// setup the display system, must happen before rendering
    void SetupDisplay(const RenderSetup& displaySetup);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// return true if display is currently setup
    bool IsDisplayValid() const;
    /// modify the display settings, may not be supported on all platforms
    void ModifyDisplay(const RenderSetup& displaySetup);
    /// process window system events (call near start of frame)
    void ProcessSystemEvents();
    /// present the current rendered frame
    void Present();
    /// check whether the window system requests to quit the application
    bool QuitRequested() const;
    
    /// get actual display attributes (can be different from DisplaySetup)
    const DisplayAttrs& GetDisplayAttrs() const;
    
    /// attach a display event handler
    void AttachDisplayEventHandler(const Ptr<Port>& handler);
    /// detach display event handler
    void DetachDisplayEventHandler(const Ptr<Port>& handler);

protected:
    /// notify event handlers, all handlers get the same message object
    void notifyEventHandlers(const Ptr<Message>& msg);

    RenderSetup renderSetup;
    DisplayAttrs displayAttrs;
    Array<Ptr<Port>> handlers;
    bool displayValid;
};
    
} // namespace _priv
} // namespace Oryol
