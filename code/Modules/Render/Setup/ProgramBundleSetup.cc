//------------------------------------------------------------------------------
//  ProgramBundleSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ProgramBundleSetup.h"
#include "Render/Core/Enums.h"

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
ProgramBundleSetup::ProgramBundleSetup(const class Locator& locator) :
Locator(locator),
numProgramEntries(0),
numUniformEntries(0) {
    // empty
}

//------------------------------------------------------------------------------
ProgramBundleSetup::programEntry&
ProgramBundleSetup::obtainEntry(uint32 mask) {
    // find existing entry with matching mask
    for (int32 i = 0; i < this->numProgramEntries; i++) {
        if (this->programEntries[i].mask == mask) {
            return this->programEntries[i];
        }
    }
    // fallthrough: return new entry
    programEntry& newEntry = this->programEntries[this->numProgramEntries++];
    newEntry.mask = mask;
    return newEntry;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddProgram(uint32 mask, const Id& vs, const Id& fs) {
    o_assert(this->numProgramEntries < MaxNumProgramEntries);
    o_assert(vs.IsValid() && vs.Type() == ResourceType::Shader);
    o_assert(fs.IsValid() && fs.Type() == ResourceType::Shader);
    
    programEntry& entry = this->obtainEntry(mask);
    entry.vertexShader = vs;
    entry.fragmentShader = fs;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddProgramFromSources(uint32 mask, ShaderLang::Code slang, const String& vsSource, const String& fsSource) {
    o_assert(this->numProgramEntries < MaxNumProgramEntries);
    o_assert(vsSource.IsValid() && fsSource.IsValid());
    o_assert_range(slang, ShaderLang::NumShaderLangs);
    
    programEntry& entry = this->obtainEntry(mask);
    entry.vsSources[slang] = vsSource;
    entry.fsSources[slang] = fsSource;
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
int32
ProgramBundleSetup::NumPrograms() const {
    return this->numProgramEntries;
}

//------------------------------------------------------------------------------
uint32
ProgramBundleSetup::Mask(int32 progIndex) const {
    o_assert_range(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].mask;
}

//------------------------------------------------------------------------------
const Id&
ProgramBundleSetup::VertexShader(int32 progIndex) const {
    o_assert_range(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].vertexShader;
}

//------------------------------------------------------------------------------
const Id&
ProgramBundleSetup::FragmentShader(int32 progIndex) const {
    o_assert_range(progIndex, this->numProgramEntries);
    return this->programEntries[progIndex].fragmentShader;
}

//------------------------------------------------------------------------------
const String&
ProgramBundleSetup::VertexShaderSource(int32 progIndex, ShaderLang::Code slang) const {
    o_assert_range(progIndex, this->numProgramEntries);
    o_assert_range(slang, ShaderLang::NumShaderLangs);
    return this->programEntries[progIndex].vsSources[slang];
}

//------------------------------------------------------------------------------
const String&
ProgramBundleSetup::FragmentShaderSource(int32 progIndex, ShaderLang::Code slang) const {
    o_assert_range(progIndex, this->numProgramEntries);
    o_assert_range(slang, ShaderLang::NumShaderLangs);
    return this->programEntries[progIndex].fsSources[slang];
}

//------------------------------------------------------------------------------
int32
ProgramBundleSetup::NumUniforms() const {
    return this->numUniformEntries;
}

//------------------------------------------------------------------------------
const String&
ProgramBundleSetup::UniformName(int32 uniformIndex) const {
    o_assert_range(uniformIndex, this->numUniformEntries);
    return this->uniformEntries[uniformIndex].uniformName;
}

//------------------------------------------------------------------------------
int16
ProgramBundleSetup::UniformSlot(int32 uniformIndex) const {
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