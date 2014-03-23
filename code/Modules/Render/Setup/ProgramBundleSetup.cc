//------------------------------------------------------------------------------
//  ProgramSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ProgramSetup.h"

namespace Oryol {
namespace Render {

using namespace Core;
    
//------------------------------------------------------------------------------
ProgramSetup::ProgramSetup() :
numCodeEntries(0),
numUniformEntries(0),
numStdUniformEntries(0) {
    // empty
}

//------------------------------------------------------------------------------
void
ProgramSetup::AddCode(uint32 mask, const String& vsSource, const String& fsSource) {
    o_assert(this->numCodeEntries < MaxNumCodeEntries);
    o_assert(vsSource.IsValid());
    o_assert(fsSource.IsValid());
    
    codeEntry& entry = this->codeEntries[this->numCodeEntries++];
    entry.mask = mask;
    entry.vsSource = vsSource;
    entry.fsSource = fsSource;
}

//------------------------------------------------------------------------------
void
ProgramSetup::AddUniform(const String& uniformName, uint32 slotIndex) {
    o_assert(this->numUniformEntries < MaxNumUniformEntries);
    o_assert(uniformName.IsValid());

    uniformEntry& entry = this->uniformEntries[this->numUniformEntries];
    entry.uniformName = uniformName;
    entry.slotIndex = slotIndex;
}

//------------------------------------------------------------------------------
void
ProgramSetup::AddStandardUniform(const String& uniformName, const StandardUniform::Code stdUniform) {
    o_assert(this->numStdUniformEntries < MaxNumStdUniformEntries);
    o_assert(uniformName.IsValid());
    o_assert(stdUniform < StandardUniform::NumStandardUniforms);

    stdUniformEntry& entry = this->stdUniformEntries[this->numStdUniformEntries];
    entry.uniformName = uniformName;
    entry.stdUniform = stdUniform;
}

} // namespace Render
} // namespace Oryol