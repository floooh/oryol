#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TBUIWindow
    @brief base class for TBUI module windows
 
    @TODO: document creating new TBUIWindow subclasses.
*/
#include "Core/Types.h"
#include "IO/Core/URL.h"
#include "tb_window.h"

namespace Oryol {

class TBUIWindow : public tb::TBWindow {
public:
    /// constructor
    TBUIWindow();
    /// destructor
    virtual ~TBUIWindow();

    /// load resource file (must be preloaded, calls LoadResource())
    void LoadResourceFile(const URL& url);
    /// generic load resource
    void LoadResource(tb::TBNode& tbNode);
};

} // namespace Oryol