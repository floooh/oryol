//------------------------------------------------------------------------------
//  glProgramBundle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glProgramBundle.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace Render {

using namespace Core;
    
//------------------------------------------------------------------------------
glProgramBundle::glProgramBundle() {
    this->clear();
}

//------------------------------------------------------------------------------
glProgramBundle::~glProgramBundle() {
    for (int32 i = 0; i < this->numProgramEntries; i++) {
        o_assert(0 == this->programEntries[i].program);
    }
}

//------------------------------------------------------------------------------
void
glProgramBundle::clear() {
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
        #if ORYOL_USE_GLGETATTRIBLOCATION
        for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
            entry.attribMapping[i] = -1;
        }
        #endif
    }
}

//------------------------------------------------------------------------------
int32
glProgramBundle::addProgram(uint32 mask, GLuint glProg) {
    o_assert(this->numProgramEntries < MaxNumPrograms);

    // make sure the mask is unique
    for (int32 i = 0; i < this->numProgramEntries; i++) {
        o_assert(this->programEntries[i].mask != mask);
    }
    
    this->programEntries[this->numProgramEntries].mask = mask;
    this->programEntries[this->numProgramEntries].program = glProg;
    return this->numProgramEntries++;
}

//------------------------------------------------------------------------------
void
glProgramBundle::bindUniform(int32 progIndex, int32 slotIndex, GLint glUniformLocation) {
    o_assert_range(progIndex, this->numProgramEntries);
    o_assert_range(slotIndex, MaxNumUniforms);
    this->programEntries[progIndex].uniformMapping[slotIndex] = glUniformLocation;
}

//------------------------------------------------------------------------------
void
glProgramBundle::bindSamplerUniform(int32 progIndex, int32 slotIndex, GLint glUniformLocation, int32 samplerIndex) {
    o_assert_range(progIndex, this->numProgramEntries);
    o_assert_range(slotIndex, MaxNumUniforms);
    this->programEntries[progIndex].uniformMapping[slotIndex] = glUniformLocation;
    this->programEntries[progIndex].samplerMapping[slotIndex] = samplerIndex;
}

//------------------------------------------------------------------------------
#if ORYOL_USE_GLGETATTRIBLOCATION
void
glProgramBundle::bindAttribLocation(int32 progIndex, VertexAttr::Code attr, GLint location) {
    o_assert_range(progIndex, this->numProgramEntries);
    o_assert_range(attr, VertexAttr::NumVertexAttrs);
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
    o_assert_range(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].program;
}

} // namespace Render
} // namespace Oryol