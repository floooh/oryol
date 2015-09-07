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
    this->numProgramEntries = 0;
    this->programEntries.Fill(programEntry());
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
int32
glShader::addProgram(uint32 mask, GLuint glProg) {
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
glShader::bindUniform(int32 progIndex, ShaderStage::Code ubBindStage, int32 ubBindSlot, int32 uniformIndex, GLint glUniformLocation) {
    this->programEntries[progIndex].uniformArrayEntry(ubBindStage, ubBindSlot, uniformIndex) = glUniformLocation;
}

//------------------------------------------------------------------------------
void
glShader::bindSampler(int32 progIndex, ShaderStage::Code bindStage, int32 texBindSlot, TextureType::Code type, int32 samplerIndex) {
    auto& smp = this->programEntries[progIndex].samplerArrayEntry(bindStage, texBindSlot);
    smp.index = samplerIndex;
    smp.type = type;
}

//------------------------------------------------------------------------------
#if ORYOL_GL_USE_GETATTRIBLOCATION
void
glShader::bindAttribLocation(int32 progIndex, VertexAttr::Code attr, GLint location) {
    this->programEntries[progIndex].attribMapping[attr] = location;
}
#endif

} // namespace _priv
} // namespace Oryol
