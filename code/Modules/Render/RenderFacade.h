#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::RenderFacade
    @brief public facade of the Render module
    
    @todo: describe RenderFacade
*/
#include "Core/Macros.h"
#include "displayMgr.h"

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
    
    /// get the original display set object
    const DisplaySetup& GetDisplaySetup() const;
    /// get the current display attributes
    const DisplayAttrs& GetDisplayAttrs() const;

private:
    displayMgr displayManager;
};
    
} // namespace Render
} // namespace Oryol