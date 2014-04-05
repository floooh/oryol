#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::renderMgrBase
    @brief base class for platform-specific renderers
*/
#include "Core/Types.h"
#include "Render/Core/mesh.h"
#include "Render/Core/texture.h"
#include "Render/Core/programBundle.h"

namespace Oryol {
namespace Render {

class displayMgr;
class stateWrapper;
    
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
    
    /// apply the current render target (can be 0)
    void ApplyRenderTarget(texture* rt);
    /// get the currently set render target
    texture* GetTexture() const;
    
    /// apply the current mesh object (can be 0)
    void ApplyMesh(mesh* mesh);
    /// get the currently set mesh object
    mesh* GetMesh() const;
    
    /// apply the current program object (can be 0)
    void ApplyProgram(programBundle* progBundle, uint32 selectionMask);
    /// get the currently set program object
    programBundle* GetProgram() const;

protected:
    bool isValid;
    displayMgr* displayManager;
    class stateWrapper* stateWrapper;
    texture* curRenderTarget;
    mesh* curMesh;
    programBundle* curProgramBundle;
};

//------------------------------------------------------------------------------
inline bool
renderMgrBase::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
inline void
renderMgrBase::ApplyRenderTarget(texture* rt) {
    o_assert_dbg(this->isValid);
    this->curRenderTarget = rt;
}

//------------------------------------------------------------------------------
inline void
renderMgrBase::ApplyMesh(mesh* msh) {
    o_assert_dbg(this->isValid);
    this->curMesh = msh;
}

//------------------------------------------------------------------------------
inline mesh*
renderMgrBase::GetMesh() const {
    o_assert_dbg(this->isValid);
    return this->curMesh;
}

//------------------------------------------------------------------------------
inline void
renderMgrBase::ApplyProgram(programBundle* prog, uint32 selMask) {
    o_assert_dbg(this->isValid);
    this->curProgramBundle = prog;
    if (nullptr != this->curProgramBundle) {
        this->curProgramBundle->selectProgram(selMask);
    }
}

//------------------------------------------------------------------------------
inline programBundle*
renderMgrBase::GetProgram() const {
    o_assert_dbg(this->isValid);
    return this->curProgramBundle;
}

} // namespace Renderer
} // namespace Oryol
 