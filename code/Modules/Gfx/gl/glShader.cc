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
    o_assert_dbg(0 == this->glProgram);
}

//------------------------------------------------------------------------------
void
glShader::Clear() {
    this->glProgram = 0;
    this->uniformMappings.Fill(-1);
    this->samplerMappings.Fill(InvalidIndex);
    #if ORYOL_GL_USE_GETATTRIBLOCATION
    this->attribMapping.Fill(-1);
    #endif
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
void
glShader::bindUniform(ShaderStage::Code bindStage, int bindSlot, int uniformIndex, GLint glUniformLocation) {
    this->uniformMappings[uniformArrayIndex(bindStage, bindSlot, uniformIndex)] = glUniformLocation;
}

//------------------------------------------------------------------------------
void
glShader::bindSampler(ShaderStage::Code bindStage, int textureIndex, int samplerIndex) {
    this->samplerMappings[samplerArrayIndex(bindStage, textureIndex)] = samplerIndex;
}

//------------------------------------------------------------------------------
#if ORYOL_GL_USE_GETATTRIBLOCATION
void
glShader::bindAttribLocation(VertexAttr::Code attr, GLint location) {
    this->attribMapping[attr] = location;
}
#endif

} // namespace _priv
} // namespace Oryol
