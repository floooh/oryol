#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TBUIWindow
    @brief base class for TBUI module windows
    
    @FIXME: document creating new TBUIWindow subclasses.
*/
#include "Core/Types.h"
#include "Core/RefCounted.h"
#include "IO/Core/IOQueue.h"
#include "Resource/ResourceLabel.h"
#include "tb_window.h"

namespace Oryol {

class TBUIWindow : public RefCounted {
    OryolClassDecl(TBUIWindow);
public:
    /// constructor
    TBUIWindow();
    /// destructor
    virtual ~TBUIWindow();

    /// preload resources, then open the window
    void Open();
    
protected:
    /// get the URL of the main resource file
    virtual URL MainResource() const;
    /// return URLs of additional resources for preloading
    virtual Array<URL> AdditionalResources() const;
    
    /// load window resource
    void LoadResource(tb::TBNode& tbNode);
    /// called when resources have been loaded
    virtual void OnResourcesLoaded();
    
    class tbWindowProxy : public tb::TBWindow, public tb::TBMessageHandler {
    public:
        // FIXME
    };
    tbWindowProxy* tbWindow;
    ResourceLabel resLabel;
    IOQueue ioQueue;
};

} // namespace Oryol