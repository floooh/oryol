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
glShader::bindUniform(int32 progIndex, int32 blockIndex, int32 slotIndex, GLint glUniformLocation) {
    this->programEntries[progIndex].uniformMappings[blockIndex][slotIndex] = glUniformLocation;
}

//------------------------------------------------------------------------------
void
glShader::bindSampler(int32 progIndex, int32 slotIndex, ShaderStage::Code stage, TextureType::Code type, int32 samplerIndex) {
    auto& smp = this->programEntries[progIndex].samplerMappings[stage][slotIndex];
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
