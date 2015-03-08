//------------------------------------------------------------------------------
//  glProgramBundle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glProgramBundle.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glProgramBundle::glProgramBundle() {
    this->Clear();
}

//------------------------------------------------------------------------------
glProgramBundle::~glProgramBundle() {
    #if !ORYOL_NO_ASSERT
    for (int32 i = 0; i < this->numProgramEntries; i++) {
        o_assert_dbg(0 == this->programEntries[i].program);
    }
    #endif
}

//------------------------------------------------------------------------------
void
glProgramBundle::Clear() {
    this->selMask = 0xFFFFFFFF;
    this->selIndex = 0;
    this->numProgramEntries = 0;
    for (int32 progIndex = 0; progIndex < MaxNumPrograms; progIndex++) {
        programEntry& entry = this->programEntries[progIndex];
        entry.mask = 0;
        entry.program = 0;
        for (int32 i = 0; i < MaxNumUniforms; i++) {
            entry.uniformMapping[i] = -1;
            entry.samplerMapping[i] = -1;
        }
        #if ORYOL_GL_USE_GETATTRIBLOCATION
        for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
            entry.attribMapping[i] = -1;
        }
        #endif
    }
    programBundleBase::Clear();
}

//------------------------------------------------------------------------------
int32
glProgramBundle::addProgram(uint32 mask, GLuint glProg) {
    o_assert_dbg(this->numProgramEntries < MaxNumPrograms);

    // make sure the mask is unique
    for (int32 i = 0; i < this->numProgramEntries; i++) {
        o_assert_dbg(this->programEntries[i].mask != mask);
    }
    
    this->programEntries[this->numProgramEntries].mask = mask;
    this->programEntries[this->numProgramEntries].program = glProg;
    return this->numProgramEntries++;
}

//------------------------------------------------------------------------------
void
glProgramBundle::bindUniform(int32 progIndex, int32 slotIndex, GLint glUniformLocation) {
    o_assert_range_dbg(progIndex, this->numProgramEntries);
    o_assert_range_dbg(slotIndex, MaxNumUniforms);
    this->programEntries[progIndex].uniformMapping[slotIndex] = glUniformLocation;
}

//------------------------------------------------------------------------------
void
glProgramBundle::bindSamplerUniform(int32 progIndex, int32 slotIndex, GLint glUniformLocation, int32 samplerIndex) {
    o_assert_range_dbg(progIndex, this->numProgramEntries);
    o_assert_range_dbg(slotIndex, MaxNumUniforms);
    this->programEntries[progIndex].uniformMapping[slotIndex] = glUniformLocation;
    this->programEntries[progIndex].samplerMapping[slotIndex] = samplerIndex;
}

//------------------------------------------------------------------------------
#if ORYOL_GL_USE_GETATTRIBLOCATION
void
glProgramBundle::bindAttribLocation(int32 progIndex, VertexAttr::Code attr, GLint location) {
    o_assert_range_dbg(progIndex, this->numProgramEntries);
    o_assert_range_dbg(attr, VertexAttr::NumVertexAttrs);
    this->programEntries[progIndex].attribMapping[attr] = location;
}
#endif

//------------------------------------------------------------------------------
int32
glProgramBundle::getNumPrograms() const {
    return this->numProgramEntries;
}

//------------------------------------------------------------------------------
GLuint
glProgramBundle::getProgramAtIndex(int32 progIndex) const {
    o_assert_range_dbg(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].program;
}

} // namespace _priv
} // namespace Oryol
