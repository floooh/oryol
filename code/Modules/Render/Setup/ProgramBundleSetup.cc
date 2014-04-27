//------------------------------------------------------------------------------
//  ProgramBundleSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ProgramBundleSetup.h"
#include "Render/Types/ResourceType.h"

namespace Oryol {
namespace Render {

using namespace Core;
using namespace Resource;
    
//------------------------------------------------------------------------------
ProgramBundleSetup::ProgramBundleSetup() :
numProgramEntries(0),
numUniformEntries(0) {
    // empty
}

//------------------------------------------------------------------------------
ProgramBundleSetup::ProgramBundleSetup(const Locator& locator) :
loc(locator),
numProgramEntries(0),
numUniformEntries(0) {
    // empty
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddProgram(uint32 mask, const Id& vs, const Id& fs) {
    o_assert(this->numProgramEntries < MaxNumProgramEntries);
    o_assert(vs.IsValid() && vs.Type() == ResourceType::Shader);
    o_assert(fs.IsValid() && fs.Type() == ResourceType::Shader);
    
    programEntry& entry = this->programEntries[this->numProgramEntries++];
    entry.mask = mask;
    entry.vertexShader = vs;
    entry.fragmentShader = fs;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddProgramFromSources(uint32 mask, const String& vsSource, const String& fsSource) {
    o_assert(this->numProgramEntries < MaxNumProgramEntries);
    o_assert(vsSource.IsValid() && fsSource.IsValid());
    
    programEntry& entry = this->programEntries[this->numProgramEntries++];
    entry.mask = mask;
    entry.vsSource = vsSource;
    entry.fsSource = fsSource;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddUniform(const String& uniformName, int16 slotIndex) {
    o_assert(this->numUniformEntries < MaxNumUniformEntries);
    o_assert(uniformName.IsValid());

    uniformEntry& entry = this->uniformEntries[this->numUniformEntries++];
    entry.uniformName = uniformName;
    entry.slotIndex = slotIndex;
    entry.isTexture = false;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddTextureUniform(const String& uniformName, int16 slotIndex) {
    o_assert(this->numUniformEntries < MaxNumUniformEntries);
    o_assert(uniformName.IsValid());

    uniformEntry& entry = this->uniformEntries[this->numUniformEntries++];
    entry.uniformName = uniformName;
    entry.slotIndex = slotIndex;
    entry.isTexture = true;
}

//------------------------------------------------------------------------------
const Locator&
ProgramBundleSetup::GetLocator() const {
    return this->loc;
}

//------------------------------------------------------------------------------
int32
ProgramBundleSetup::GetNumPrograms() const {
    return this->numProgramEntries;
}

//------------------------------------------------------------------------------
uint32
ProgramBundleSetup::GetMask(int32 progIndex) const {
    o_assert_range(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].mask;
}

//------------------------------------------------------------------------------
const Id&
ProgramBundleSetup::GetVertexShader(int32 progIndex) const {
    o_assert_range(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].vertexShader;
}

//------------------------------------------------------------------------------
const Id&
ProgramBundleSetup::GetFragmentShader(int32 progIndex) const {
    o_assert_range(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].fragmentShader;
}

//------------------------------------------------------------------------------
const String&
ProgramBundleSetup::GetVertexShaderSource(int32 progIndex) const {
    o_assert_range(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].vsSource;
}

//------------------------------------------------------------------------------
const String&
ProgramBundleSetup::GetFragmentShaderSource(int32 progIndex) const {
    o_assert_range(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].fsSource;
}

//------------------------------------------------------------------------------
int32
ProgramBundleSetup::GetNumUniforms() const {
    return this->numUniformEntries;
}

//------------------------------------------------------------------------------
const String&
ProgramBundleSetup::GetUniformName(int32 uniformIndex) const {
    o_assert_range(uniformIndex, this->numUniformEntries);
    return this->uniformEntries[uniformIndex].uniformName;
}

//------------------------------------------------------------------------------
int16
ProgramBundleSetup::GetUniformSlot(int32 uniformIndex) const {
    o_assert_range(uniformIndex, this->numUniformEntries);
    return this->uniformEntries[uniformIndex].slotIndex;
}

//------------------------------------------------------------------------------
bool
ProgramBundleSetup::IsTextureUniform(int32 uniformIndex) const {
    o_assert_range(uniformIndex, this->numUniformEntries);
    return this->uniformEntries[uniformIndex].isTexture;
}

} // namespace Render
} // namespace Oryol