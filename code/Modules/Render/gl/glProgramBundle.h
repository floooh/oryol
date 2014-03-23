#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glProgramBundle
    @brief private: GL implementation of program bundle
*/
#include "Render/base/programBundleBase.h"
#include "Render/Types/StandardUniform.h"
#include "Core/Assert.h"
#include "Render/gl/gl_decl.h"

namespace Oryol {
namespace Render {
    
class glProgramBundle : public programBundleBase {
public:
    /// constructor
    glProgramBundle();
    /// destructor
    ~glProgramBundle();
    
    /// clear the object
    void clear();
    
    /// set GL program object by mask
    int32 addProgram(uint32 mask, GLuint glProg);
    /// bind a uniform location to a slot index
    void bindUniform(int32 progIndex, int32 slotIndex, GLint glUniformLocation);
    /// bind a uniform location to a standard uniform
    void bindStandardUniform(int32 progIndex, StandardUniform::Code stdUniform, GLint glUniformLocation);
    
    /// select program in the bundle
    bool selectProgram(uint32 mask);
    /// get the current selection mask
    uint32 getSelectionMask() const;
    /// get the currently selected GL program
    GLuint getProgram() const;
    /// get uniform location by slot index in currently selected program (-1 if not exists)
    GLint getUniformLocation(int32 slotIndex) const;
    /// get uniform location by slot index in currently selected program (-1 if not exists)
    GLint getStandardUniformLocation(StandardUniform::Code stdUniform) const;
    
    /// get number of programs
    int32 getNumPrograms() const;
    /// get program at index
    GLuint getProgramAtIndex(int32 progIndex) const;
    
private:
    static const int32 MaxNumUniforms = 16;
    static const int32 MaxNumPrograms = 8;

    struct programEntry {
        uint32 mask;
        GLuint program;
        GLint stdUniformMapping[StandardUniform::NumStandardUniforms];
        GLint uniformMapping[MaxNumUniforms];
    };
    uint32 selMask;
    int32 selIndex;
    int32 numProgramEntries;
    programEntry programEntries[MaxNumPrograms];
};

//------------------------------------------------------------------------------
inline uint32
glProgramBundle::getSelectionMask() const {
    return this->selMask;
}

//------------------------------------------------------------------------------
inline bool
glProgramBundle::selectProgram(uint32 mask) {
    // number of programs will be small, so linear is ok
    if (this->selMask != mask) {
        for (int32 i = 0; i < this->numProgramEntries; i++) {
            if (this->programEntries[i].mask == mask) {
                this->selMask = mask;
                this->selIndex = i;
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
inline GLuint
glProgramBundle::getProgram() const {
    return this->programEntries[this->selIndex].program;
}

//------------------------------------------------------------------------------
inline GLint
glProgramBundle::getUniformLocation(int32 slotIndex) const {
    o_assert_range_dbg(slotIndex, MaxNumUniforms);
    return this->programEntries[this->selIndex].uniformMapping[slotIndex];
}

//------------------------------------------------------------------------------
inline GLint
glProgramBundle::getStandardUniformLocation(StandardUniform::Code stdUniform) const {
    o_assert_range_dbg(stdUniform, StandardUniform::NumStandardUniforms);
    return this->programEntries[this->selIndex].stdUniformMapping[stdUniform];
}
    
} // namespace Render
} // namespace Oryol