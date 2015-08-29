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
    void bindUniform(int32 progIndex, int32 blockIndex, int32 slotIndex, GLint glUniformLocation);
    /// bind a sampler uniform location to a slot index
    void bindSamplerUniform(int32 progIndex, int32 blockIndex, int32 slotIndex, GLint glUniformLocation, int32 samplerIndex);
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
    GLint getUniformLocation(int32 progIndex, int32 blockIndex, int32 slotIndex) const;
    /// get sampler location by slot index in program (-1 if not exists)
    int32 getSamplerIndex(int32 progIndex, int32 blockIndex, int32 slotIndex) const;
    #if ORYOL_GL_USE_GETATTRIBLOCATION
    /// get a vertex attribute location
    GLint getAttribLocation(int32 progIndex, VertexAttr::Code attrib) const;
    #endif
        
private:
    static const int32 MaxNumUniformBlocks = 4;
    static const int32 MaxNumUniforms = 16;
    static const int32 MaxNumPrograms = 8;

    struct mapping {
        GLint uniform = -1;
        int32 sampler = -1;
    };

    struct programEntry {
        programEntry() :
            mask(0),
            program(0) {
            #if ORYOL_GL_USE_GETATTRIBLOCATION
            this->attribMapping.Fill(-1);
            #endif
        };
        uint32 mask = 0;
        GLuint program = 0;
        StaticArray<StaticArray<mapping,MaxNumUniforms>,MaxNumUniformBlocks> mappings;
        #if ORYOL_GL_USE_GETATTRIBLOCATION
        StaticArray<GLint,VertexAttr::NumVertexAttrs> attribMapping;
        #endif
    };
    int32 numProgramEntries;
    StaticArray<programEntry, MaxNumPrograms> programEntries;
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
glShader::getUniformLocation(int32 progIndex, int32 blockIndex, int32 slotIndex) const {
    return this->programEntries[progIndex].mappings[blockIndex][slotIndex].uniform;
}

//------------------------------------------------------------------------------
inline int32
glShader::getSamplerIndex(int32 progIndex, int32 blockIndex, int32 slotIndex) const {
    return this->programEntries[progIndex].mappings[blockIndex][slotIndex].sampler;
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
