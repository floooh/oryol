//------------------------------------------------------------------------------
//  glRenderMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glRenderMgr.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
bool
glRenderMgr::ApplyMesh(mesh* msh) {
    if (renderMgrBase::ApplyMesh(msh)) {
        GLuint vao = msh->glGetVertexArrayObject();
        o_assert_dbg(0 != vao);
        this->stateWrapper->glBindVertexArrayObject(vao);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool
glRenderMgr::ApplyProgram(programBundle* progBundle, uint32 selMask) {
    if (renderMgrBase::ApplyProgram(progBundle, selMask)) {
        GLuint glProg = progBundle->getProgram();
        o_assert_dbg(0 != glProg);
        this->stateWrapper->glUseProgram(glProg);
        return true;
    }
    else {
        return false;
    }
}

} // namespace Render
} // namespace Oryol