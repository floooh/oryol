#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::displayMgrBase
    @brief private: display managers base class
    
    @see displayMgr
*/
#include "Render/DisplaySetup.h"
#include "Render/DisplayAttrs.h"
#include "Core/Containers/Array.h"
#include "Messaging/Port.h"

namespace Oryol {
namespace Render {
    
class displayMgrBase {
public:
    /// constructor
    displayMgrBase();
    /// destructor
    ~displayMgrBase();
    
    /// setup the display system, must happen before rendering
    void SetupDisplay(const DisplaySetup& displaySetup);
    /// discard the display, rendering cannot happen after
    void DiscardDisplay();
    /// return true if display is currently setup
    bool IsDisplayValid() const;
    /// modify the display settings, may not be supported on all platforms
    void ModifyDisplay(const DisplaySetup& displaySetup);
    /// process window system events (call near start of frame)
    void ProcessSystemEvents();
    /// present the current rendered frame
    void Present();
    
    /// get original display setup settings
    const DisplaySetup& GetDisplaySetup() const;
    /// get actual display attributes (can be different from DisplaySetup)
    const DisplayAttrs& GetDisplayAttrs() const;
    
    /// attach a display event handler
    void AttachDisplayEventHandler(const Core::Ptr<Messaging::Port>& handler);
    /// detach display event handler
    void DetachDisplayEventHandler(const Core::Ptr<Messaging::Port>& handler);

protected:
    /// notify event handlers, all handlers get the same message object
    void notifyEventHandlers(const Core::Ptr<Messaging::Message>& msg);

    DisplaySetup displaySetup;
    DisplayAttrs displayAttrs;
    Core::Array<Core::Ptr<Messaging::Port>> handlers;
    bool displayValid;
};
    
} // namespace Render
} // namespace Oryol