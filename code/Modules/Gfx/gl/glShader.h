#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glShader
    @ingroup _priv
    @brief private: GL implementation of shader
*/
#include "Core/Containers/StaticArray.h"
#include "Gfx/Resource/shaderBase.h"
#include "Core/Assertion.h"
#include "Gfx/gl/gl_decl.h"

namespace Oryol {
namespace _priv {
    
class glShader : public shaderBase {
public:
    /// constructor
    glShader();
    /// destructor
    ~glShader();
    
    /// clear the object
    void Clear();
    
    /// bind a uniform location to a slot index
    void bindUniform(ShaderStage::Code bindStage, int32 bindSlot, int32 uniformIndex, GLint glUniformLocation);
    /// bind a sampler uniform location to a slot index
    void bindSampler(ShaderStage::Code bindStage, int32 textureIndex, int32 samplerIndex);
    #if ORYOL_GL_USE_GETATTRIBLOCATION
    /// bind a vertex attribute location
    void bindAttribLocation(VertexAttr::Code attrib, GLint attribLocation);
    #endif
    
    /// get uniform location (-1 if not exists)
    GLint getUniformLocation(ShaderStage::Code bindStage, int32 bindSlot, int32 uniformIndex) const;
    /// get sampler index (InvalidIndex if not exists)
    int32 getSamplerIndex(ShaderStage::Code bindStage, int32 textureIndex) const;
    #if ORYOL_GL_USE_GETATTRIBLOCATION
    /// get a vertex attribute location
    GLint getAttribLocation(VertexAttr::Code attrib) const;
    #endif

    /// compute uniform array index
    static int32 uniformArrayIndex(ShaderStage::Code bindStage, int32 bindSlot, int32 uniformIndex);
    /// compute sampler array index
    static int32 samplerArrayIndex(ShaderStage::Code bindStage, int32 textureIndex);

    /// the GL shader program
    GLuint glProgram;

private:
    static const int32 MaxUniformsPerBlock = GfxConfig::MaxNumUniformBlockLayoutComponents;
    static const int32 MaxTexturesPerBlock = GfxConfig::MaxNumTextureBlockLayoutComponents;
    static const int32 MaxUBsPerStage = GfxConfig::MaxNumUniformBlocksPerStage;
    static const int32 MaxStages = ShaderStage::NumShaderStages;

    StaticArray<GLint, MaxStages*MaxUBsPerStage*MaxUniformsPerBlock> uniformMappings;
    StaticArray<int32, MaxStages*MaxTexturesPerBlock> samplerMappings;
    #if ORYOL_GL_USE_GETATTRIBLOCATION
    StaticArray<GLint,VertexAttr::NumVertexAttrs> attribMapping;
    #endif
};

//------------------------------------------------------------------------------
inline int32
glShader::uniformArrayIndex(ShaderStage::Code bindStage, int32 bindSlot, int32 uniformIndex) {
    return uniformIndex + bindSlot*MaxUniformsPerBlock + bindStage*MaxUBsPerStage*MaxUniformsPerBlock;
}

//------------------------------------------------------------------------------
inline int32
glShader::samplerArrayIndex(ShaderStage::Code bindStage, int32 textureIndex) {
    return textureIndex + bindStage*MaxTexturesPerBlock;
}

//------------------------------------------------------------------------------
inline GLint
glShader::getUniformLocation(ShaderStage::Code bindStage, int32 bindSlot, int32 uniformIndex) const {
    return this->uniformMappings[uniformArrayIndex(bindStage, bindSlot, uniformIndex)];
}

//------------------------------------------------------------------------------
inline int32
glShader::getSamplerIndex(ShaderStage::Code bindStage, int32 textureIndex) const {
    return this->samplerMappings[samplerArrayIndex(bindStage, textureIndex)];
}

//------------------------------------------------------------------------------
#if ORYOL_GL_USE_GETATTRIBLOCATION
inline GLint
glShader::getAttribLocation(VertexAttr::Code attrib) const {
    return this->attribMapping[attrib];
}
#endif
    
} // namespace _priv
} // namespace Oryol
