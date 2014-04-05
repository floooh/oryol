//------------------------------------------------------------------------------
//  glRenderMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glRenderMgr.h"
#include "Render/Core/displayMgr.h"
#include "Render/Core/stateWrapper.h"
#include "Render/gl/glTypes.h"
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
void
glRenderMgr::ApplyRenderTarget(texture* rt) {
    renderMgrBase::ApplyRenderTarget(rt);
    if (nullptr == rt) {
        // bind default render target
        ::glBindFramebuffer(GL_FRAMEBUFFER, this->displayManager->glGetDefaultFramebuffer());
        ORYOL_GL_CHECK_ERROR();
    }
    else {
        ::glBindFramebuffer(GL_FRAMEBUFFER, rt->glGetFramebuffer());
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glRenderMgr::ApplyMesh(mesh* msh) {
    renderMgrBase::ApplyMesh(msh);
    this->stateWrapper->BindMesh(msh);
}

//------------------------------------------------------------------------------
void
glRenderMgr::ApplyProgram(programBundle* progBundle, uint32 selMask) {
    renderMgrBase::ApplyProgram(progBundle, selMask);
    this->stateWrapper->BindProgram(progBundle);
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

//------------------------------------------------------------------------------
void
glRenderMgr::Clear(bool color, bool depth, bool stencil) {
    o_assert_dbg(this->isValid);

    GLbitfield clearMask = 0;
    if (color) {
        clearMask |= GL_COLOR_BUFFER_BIT;
    }
    if (depth) {
        clearMask |= GL_DEPTH_BUFFER_BIT;
    }
    if (stencil) {
        clearMask |= GL_STENCIL_BUFFER_BIT;
    }
    ::glClear(clearMask);
    ORYOL_GL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
glRenderMgr::Draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->curMesh);
    
    const GLenum glPrimType = glTypes::AsGLPrimitiveType(primGroup.GetPrimitiveType());
    const IndexType::Code indexType = this->curMesh->GetIndexBufferAttrs().GetIndexType();
    if (indexType != IndexType::None) {
        // indexed geometry
        const int32 indexByteSize = IndexType::ByteSize(indexType);
        GLenum glIndexType = glTypes::AsGLIndexType(indexType);
        const GLvoid* indices = (const GLvoid*) (GLintptr) (primGroup.GetBaseElement() * indexByteSize);
        ::glDrawElements(glPrimType, primGroup.GetNumElements(), glIndexType, indices);
        ORYOL_GL_CHECK_ERROR();
    }
    else {
        // non-indexed geometry
        ::glDrawArrays(glPrimType, primGroup.GetBaseElement(), primGroup.GetNumElements());
        ORYOL_GL_CHECK_ERROR();
    }
}

//------------------------------------------------------------------------------
void
glRenderMgr::Draw(int32 primGroupIndex) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->curMesh);
    
    if (primGroupIndex >= this->curMesh->GetNumPrimitiveGroups()) {
        // this may happen if trying to render a placeholder which doesn't
        // have as many materials as the original mesh, anyway, this isn't
        // a serious error
        return;
    }
    const PrimitiveGroup& primGroup = this->curMesh->GetPrimitiveGroup(primGroupIndex);
    this->Draw(primGroup);
}

} // namespace Render
} // namespace Oryol
