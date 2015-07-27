//------------------------------------------------------------------------------
//  glShader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glShader.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glShader::glShader() {
    this->Clear();
}

//------------------------------------------------------------------------------
glShader::~glShader() {
    #if !ORYOL_NO_ASSERT
    for (int32 i = 0; i < this->numProgramEntries; i++) {
        o_assert_dbg(0 == this->programEntries[i].program);
    }
    #endif
}

//------------------------------------------------------------------------------
void
glShader::Clear() {
    this->selMask = 0xFFFFFFFF;
    this->selIndex = 0;
    this->numProgramEntries = 0;
    for (auto& entry : this->programEntries) {
        entry.mask = 0;
        entry.program = 0;
        for (int32 blockIndex = 0; blockIndex < MaxNumUniformBlocks; blockIndex++) {
            for (int32 uniformIndex = 0; uniformIndex < MaxNumUniforms; uniformIndex++) {
                entry.uniformMapping[blockIndex][uniformIndex] = -1;
                entry.samplerMapping[blockIndex][uniformIndex] = -1;
            }
        }
        #if ORYOL_GL_USE_GETATTRIBLOCATION
        for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
            entry.attribMapping[i] = -1;
        }
        #endif
    }
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
int32
glShader::addProgram(uint32 mask, GLuint glProg) {
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
glShader::bindUniform(int32 progIndex, int32 blockIndex, int32 slotIndex, GLint glUniformLocation) {
    o_assert_range_dbg(progIndex, this->numProgramEntries);
    o_assert_range_dbg(blockIndex, MaxNumUniformBlocks);
    o_assert_range_dbg(slotIndex, MaxNumUniforms);
    this->programEntries[progIndex].uniformMapping[blockIndex][slotIndex] = glUniformLocation;
}

//------------------------------------------------------------------------------
void
glShader::bindSamplerUniform(int32 progIndex, int32 blockIndex, int32 slotIndex, GLint glUniformLocation, int32 samplerIndex) {
    o_assert_range_dbg(progIndex, this->numProgramEntries);
    o_assert_range_dbg(slotIndex, MaxNumUniformBlocks);
    o_assert_range_dbg(slotIndex, MaxNumUniforms);
    this->programEntries[progIndex].uniformMapping[blockIndex][slotIndex] = glUniformLocation;
    this->programEntries[progIndex].samplerMapping[blockIndex][slotIndex] = samplerIndex;
}

//------------------------------------------------------------------------------
#if ORYOL_GL_USE_GETATTRIBLOCATION
void
glShader::bindAttribLocation(int32 progIndex, VertexAttr::Code attr, GLint location) {
    o_assert_range_dbg(progIndex, this->numProgramEntries);
    o_assert_range_dbg(attr, VertexAttr::NumVertexAttrs);
    this->programEntries[progIndex].attribMapping[attr] = location;
}
#endif

//------------------------------------------------------------------------------
int32
glShader::getNumPrograms() const {
    return this->numProgramEntries;
}

//------------------------------------------------------------------------------
GLuint
glShader::getProgramAtIndex(int32 progIndex) const {
    o_assert_range_dbg(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].program;
}

} // namespace _priv
} // namespace Oryol
