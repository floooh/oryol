//------------------------------------------------------------------------------
//  glRenderMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glRenderMgr.h"
#include "Render/gl/gl_impl.h"
#define GLM_FORCE_RADIANS
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

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

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const float32& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform1f(glLoc, val);
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::vec2& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform2f(glLoc, val.x, val.y);
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::vec3& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform3f(glLoc, val.x, val.y, val.z);
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::vec4& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform4f(glLoc, val.x, val.y, val.z, val.w);
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const int32& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform1i(glLoc, val);
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::ivec2& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform2i(glLoc, val.x, val.y);
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::ivec3& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform3i(glLoc, val.x, val.y, val.z);
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::ivec4& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform4i(glLoc, val.x, val.y, val.z, val.w);
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::mat4& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniformMatrix4fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::mat3& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniformMatrix3fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariable(int32 index, const glm::mat2& val) {
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniformMatrix2fv(glLoc, 1, GL_FALSE, glm::value_ptr(val));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const float32* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform1fv(glLoc, numValues, values);
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::vec2* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform2fv(glLoc, numValues, glm::value_ptr(*values));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::vec3* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform3fv(glLoc, numValues, glm::value_ptr(*values));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::vec4* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform4fv(glLoc, numValues, glm::value_ptr(*values));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const int32* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform1iv(glLoc, numValues, values);
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::ivec2* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform2iv(glLoc, numValues, glm::value_ptr(*values));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::ivec3* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform3iv(glLoc, numValues, glm::value_ptr(*values));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::ivec4* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniform4iv(glLoc, numValues, glm::value_ptr(*values));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::mat4* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniformMatrix4fv(glLoc, numValues, GL_FALSE, glm::value_ptr(*values));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::mat3* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniformMatrix3fv(glLoc, numValues, GL_FALSE, glm::value_ptr(*values));
    }
}

//------------------------------------------------------------------------------
template<> void
glRenderMgr::ApplyVariableArray(int32 index, const glm::mat2* values, int32 numValues) {
    o_assert_dbg(values && (numValues > 0));
    if (this->curProgramBundle) {
        GLint glLoc = this->curProgramBundle->getUniformLocation(index);
        ::glUniformMatrix2fv(glLoc, numValues, GL_FALSE, glm::value_ptr(*values));
    }
}

} // namespace Render
} // namespace Oryol