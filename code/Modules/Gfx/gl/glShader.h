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
    
    /// set GL program object by mask
    int32 addProgram(uint32 mask, GLuint glProg);
    /// bind a uniform location to a slot index
    void bindUniform(int32 progIndex, ShaderStage::Code ubBindStage, int32 ubBindSlot, int32 uniformIndex, GLint glUniformLocation);
    /// bind a sampler uniform location to a slot index
    void bindSampler(int32 progIndex, ShaderStage::Code texBindStage, int32 texBindSlot, TextureType::Code type, int32 samplerIndex);
    #if ORYOL_GL_USE_GETATTRIBLOCATION
    /// bind a vertex attribute location
    void bindAttribLocation(int32 progIndex, VertexAttr::Code attrib, GLint attribLocation);
    #endif
    
    /// get program index by mask (returns InvalidIndex if not exists)
    int32 getProgIndexByMask(uint32 mask) const;
    /// get number of programs
    int32 getNumPrograms() const;
    /// get program at index
    GLuint getProgram(int32 progIndex) const;
    /// get uniform location by slot index in program (-1 if not exists)
    GLint getUniformLocation(int32 progIndex, ShaderStage::Code ubBindStage, int32 ubBindSlot, int32 uniformIndex) const;
    /// get sampler location by slot index in program (-1 if not exists)
    int32 getSamplerIndex(int32 progIndex, ShaderStage::Code texBindStage, int32 texBindSlot) const;
    /// get sampler type by slot index in program
    TextureType::Code getSamplerType(int32 progIndex, ShaderStage::Code texBindStage, int32 texBindSlot) const;
    #if ORYOL_GL_USE_GETATTRIBLOCATION
    /// get a vertex attribute location
    GLint getAttribLocation(int32 progIndex, VertexAttr::Code attrib) const;
    #endif
        
private:
    static const int32 MaxUniformsPerBlock = 16;
    static const int32 MaxUBsPerStage = 4;
    static const int32 MaxStages = ShaderStage::NumShaderStages;
    static const int32 MaxPrograms = 4;
    static const int32 MaxSamplers = GfxConfig::MaxNumShaderTextures;


    struct programEntry {
        //======================================================================
        // NOTE:
        // The GL renderer doesn't support uniform blocks yet (but even if
        // it did we need a solution for GLES2/WebGL). This programEntry struct
        // keeps a big mapping of uniforms inside Oryol's uniform block
        // structures to granular GLSL uniforms. The mapping is as this:
        // There are up to MaxProgram shader programs in the shader object,
        // each with:
        //  - up to MaxStage shader stages (exactly 2: Vertex- and Fragment-Shader)
        //    each stage has:
        //      - up to MaxUBsPerStage uniform blocks, and each uniform block has
        //          - up to MaxUniformsPerBlock uniform values embedded
        // Thus a uniform is addresses by:
        //  [progIndex][ShaderStage][UniformBlockBindSlot][UniformIndex]
        //
        // There is a similar mapping for texture samplers, but much simpler:
        //  [progIndex][ShaderStage][TextureBindSlot]
        // The number of texture bind slots per shader stage is different:
        // The vertex shader stage has GfxConfig::MaxNumVSTextures texture
        // bind slots, and the fragment shader stage has GfxConfig::MaxNumFSTextures
        // texture bind slots.
        //======================================================================

        struct sampler {
            int32 index = InvalidIndex;
            TextureType::Code type = TextureType::InvalidTextureType;
        };
        programEntry() :
            mask(0),
            program(0) {
            this->uniformMappings.Fill(-1);
            this->samplerMappings.Fill(sampler());
            #if ORYOL_GL_USE_GETATTRIBLOCATION
            this->attribMapping.Fill(-1);
            #endif
        };
        /// access to uniformArrayEntry
        GLint& uniformArrayEntry(ShaderStage::Code ubBindStage, int32 ubBindSlot, int32 uniformIndex) {
            return this->uniformMappings[
                    uniformIndex +
                    ubBindSlot * MaxUniformsPerBlock +
                    ubBindStage * MaxUBsPerStage * MaxUniformsPerBlock];
        };
        /// array to sampler array entry
        sampler& samplerArrayEntry(ShaderStage::Code bindStage, int32 bindSlot) {
            return this->samplerMappings[
                    bindSlot +
                    bindStage == ShaderStage::VS ? 0 : GfxConfig::MaxNumVSTextures];
        };
        uint32 mask = 0;
        GLuint program = 0;
        StaticArray<GLint, MaxStages*MaxUBsPerStage*MaxUniformsPerBlock> uniformMappings;
        StaticArray<sampler, MaxSamplers> samplerMappings;
        #if ORYOL_GL_USE_GETATTRIBLOCATION
        StaticArray<GLint,VertexAttr::NumVertexAttrs> attribMapping;
        #endif
    };
    int32 numProgramEntries;
    StaticArray<programEntry, MaxPrograms> programEntries;
};

//------------------------------------------------------------------------------
inline int32
glShader::getProgIndexByMask(uint32 mask) const {
    // number of programs will be small, so linear is ok
    for (int32 i = 0; i < this->numProgramEntries; i++) {
        if (this->programEntries[i].mask == mask) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
inline int32
glShader::getNumPrograms() const {
    return this->numProgramEntries;
}

//------------------------------------------------------------------------------
inline GLuint
glShader::getProgram(int32 progIndex) const {
    return this->programEntries[progIndex].program;
}

//------------------------------------------------------------------------------
inline GLint
glShader::getUniformLocation(int32 progIndex, ShaderStage::Code ubBindStage, int32 ubBindSlot, int32 uniformIndex) const {
    return const_cast<glShader*>(this)->programEntries[progIndex].uniformArrayEntry(ubBindStage, ubBindSlot, uniformIndex);
}

//------------------------------------------------------------------------------
inline int32
glShader::getSamplerIndex(int32 progIndex, ShaderStage::Code texBindStage, int32 texBindSlot) const {
    return const_cast<glShader*>(this)->programEntries[progIndex].samplerArrayEntry(texBindStage, texBindSlot).index;
}

//------------------------------------------------------------------------------
inline TextureType::Code
glShader::getSamplerType(int32 progIndex, ShaderStage::Code texBindStage, int32 texBindSlot) const {
    return const_cast<glShader*>(this)->programEntries[progIndex].samplerArrayEntry(texBindStage, texBindSlot).type;
}

//------------------------------------------------------------------------------
#if ORYOL_GL_USE_GETATTRIBLOCATION
inline GLint
glShader::getAttribLocation(int32 progIndex, VertexAttr::Code attrib) const {
    return this->programEntries[progIndex].attribMapping[attrib];
}
#endif
    
} // namespace _priv
} // namespace Oryol
