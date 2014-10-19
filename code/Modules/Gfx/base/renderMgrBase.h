#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::renderMgrBase
    @ingroup _priv
    @brief base class for platform-specific renderers
*/
#include "Core/Types.h"
#include "Gfx/Core/mesh.h"
#include "Gfx/Attrs/DisplayAttrs.h"

namespace Oryol {
namespace _priv {

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
    /// get the current render target attributes
    const DisplayAttrs& GetRenderTargetAttrs() const;
    
    /// apply the current render target (can be 0)
    void ApplyRenderTarget(texture* rt);
    /// apply the current draw state
    void ApplyDrawState(drawState* ds);
    /// commit current frame
    void CommitFrame();
    
protected:
    bool isValid;
    bool renderTargetValid;
    displayMgr* displayManager;
    class stateWrapper* stateWrapper;
    texture* curRenderTarget;
    drawState* curDrawState;
    mesh* curMesh;
    programBundle* curProgramBundle;
    DisplayAttrs renderTargetAttrs;
};

//------------------------------------------------------------------------------
inline bool
renderMgrBase::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
inline const DisplayAttrs&
renderMgrBase::GetRenderTargetAttrs() const {
    return this->renderTargetAttrs;
}

} // namespace _priv
} // namespace Oryol
 