#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::renderMgrBase
    @brief base class for platform-specific renderers
*/
#include "Core/Types.h"
#include "Render/Core/mesh.h"
#include "Render/Core/programBundle.h"
#include "Render/Core/stateWrapper.h"

namespace Oryol {
namespace Render {
    
class renderMgrBase {
public:
    /// constructor
    renderMgrBase();
    /// destructor
    ~renderMgrBase();
    
    /// setup the renderer
    void Setup(stateWrapper* stateWrapper);
    /// discard the renderer
    void Discard();
    /// return true if the renderer has been setup
    bool IsValid() const;
    
    /// apply the current mesh object
    bool ApplyMesh(mesh* mesh);
    /// invalidate the currently set mesh object
    void ClearMesh();
    /// get the currently set mesh object
    mesh* GetMesh() const;
    
    /// apply the current program object
    bool ApplyProgram(programBundle* progBundle, uint32 selectionMask);
    /// invalidate the currently set program object
    void ClearProgram();
    /// get the currently set program object
    programBundle* GetProgram() const;

protected:
    bool isValid;
    class stateWrapper* stateWrapper;
    mesh* curMesh;
    programBundle* curProgramBundle;
};

//------------------------------------------------------------------------------
inline bool
renderMgrBase::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
inline bool
renderMgrBase::ApplyMesh(mesh* msh) {
    o_assert_dbg(this->isValid && (nullptr != msh));
    if (msh != this->curMesh) {
        this->curMesh = msh;
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
inline void
renderMgrBase::ClearMesh() {
    o_assert_dbg(this->isValid);
    this->stateWrapper->InvalidateMeshState();
    this->curMesh = nullptr;
}

//------------------------------------------------------------------------------
inline mesh*
renderMgrBase::GetMesh() const {
    o_assert_dbg(this->isValid);
    return this->curMesh;
}

//------------------------------------------------------------------------------
inline bool
renderMgrBase::ApplyProgram(programBundle* prog, uint32 selMask) {
    o_assert_dbg(this->isValid && (nullptr != prog));
    if (prog != this->curProgramBundle) {
        this->curProgramBundle = prog;
        this->curProgramBundle->selectProgram(selMask);
        return true;
    }
    else {
        if (this->curProgramBundle->getSelectionMask() != selMask) {
            this->curProgramBundle->selectProgram(selMask);
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
inline void
renderMgrBase::ClearProgram() {
    o_assert_dbg(this->isValid);
    this->stateWrapper->InvalidateProgramState();
    this->curProgramBundle = nullptr;
}

//------------------------------------------------------------------------------
inline programBundle*
renderMgrBase::GetProgram() const {
    o_assert_dbg(this->isValid);
    return this->curProgramBundle;
}

} // namespace Renderer
} // namespace Oryol
 