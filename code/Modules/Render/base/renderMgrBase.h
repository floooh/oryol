#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::renderMgrBase
    @brief base class for platform-specific renderers
*/
#include "Core/Types.h"
#include "Render/Core/mesh.h"

namespace Oryol {
namespace Render {

class displayMgr;
class stateWrapper;
class drawState;
class mesh;
class programBundle;
class texture;
    
class renderMgrBase {
public:
    /// constructor
    renderMgrBase();
    /// destructor
    ~renderMgrBase();
    
    /// setup the renderer
    void Setup(stateWrapper* stateWrapper, displayMgr* dispMgr);
    /// discard the renderer
    void Discard();
    /// return true if the renderer has been setup
    bool IsValid() const;
    
    /// called at begin of frame
    void BeginFrame();
    /// called at end of frame
    void EndFrame();
    /// apply the current render target (can be 0)
    void ApplyRenderTarget(texture* rt);
    /// apply the current draw state
    void ApplyDrawState(drawState* ds);
    
protected:
    bool isValid;
    bool renderTargetValid;
    bool inFrame;
    displayMgr* displayManager;
    class stateWrapper* stateWrapper;
    texture* curRenderTarget;
    drawState* curDrawState;
    mesh* curMesh;
    programBundle* curProgramBundle;
};

//------------------------------------------------------------------------------
inline bool
renderMgrBase::IsValid() const {
    return this->isValid;
}

} // namespace Renderer
} // namespace Oryol
 