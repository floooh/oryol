//------------------------------------------------------------------------------
//  ProgramBundleSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ProgramBundleSetup.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {

//------------------------------------------------------------------------------
ProgramBundleSetup::ProgramBundleSetup() :
Locator(Locator::NonShared()),
numProgramEntries(0),
numUniformBlockEntries(0) {
    // empty
}

//------------------------------------------------------------------------------
ProgramBundleSetup::ProgramBundleSetup(const class Locator& locator) :
Locator(locator),
numProgramEntries(0),
numUniformBlockEntries(0) {
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
    o_assert_dbg(this->numProgramEntries < MaxNumProgramEntries);
    o_assert_dbg(vs.IsValid() && vs.Type == GfxResourceType::Shader);
    o_assert_dbg(fs.IsValid() && fs.Type == GfxResourceType::Shader);
    
    programEntry& entry = this->obtainEntry(mask);
    entry.vertexShader = vs;
    entry.fragmentShader = fs;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddProgramFromSources(uint32 mask, ShaderLang::Code slang, const String& vsSource, const String& fsSource) {
    o_assert_dbg(this->numProgramEntries < MaxNumProgramEntries);
    o_assert_dbg(vsSource.IsValid() && fsSource.IsValid());

    programEntry& entry = this->obtainEntry(mask);
    entry.vsSources[slang] = vsSource;
    entry.fsSources[slang] = fsSource;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddProgramFromByteCode(uint32 mask, ShaderLang::Code slang, const uint8* vsByteCode, uint32 vsNumBytes, const uint8* fsByteCode, uint32 fsNumBytes) {
    o_assert_dbg(this->numProgramEntries < MaxNumProgramEntries);
    o_assert_dbg(vsByteCode && (vsNumBytes > 0));
    o_assert_dbg(fsByteCode && (fsNumBytes > 0));

    programEntry& entry = this->obtainEntry(mask);
    entry.vsByteCode[slang].ptr = vsByteCode;
    entry.vsByteCode[slang].size = vsNumBytes;
    entry.fsByteCode[slang].ptr = fsByteCode;
    entry.fsByteCode[slang].size = fsNumBytes;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddUniformBlock(const StringAtom& name, const UniformLayout& layout, int16 slotIndex) {
    o_assert_dbg(name.IsValid());
    o_assert_dbg(!layout.Empty());

    uniformBlockEntry& entry = this->uniformBlockEntries[this->numUniformBlockEntries++];
    entry.name = name;
    entry.layout = layout;
    entry.slotIndex = slotIndex;
}

//------------------------------------------------------------------------------
int32
ProgramBundleSetup::NumPrograms() const {
    return this->numProgramEntries;
}

//------------------------------------------------------------------------------
uint32
ProgramBundleSetup::Mask(int32 progIndex) const {
    return this->programEntries[progIndex].mask;
}

//------------------------------------------------------------------------------
const Id&
ProgramBundleSetup::VertexShader(int32 progIndex) const {
    return this->programEntries[progIndex].vertexShader;
}

//------------------------------------------------------------------------------
const Id&
ProgramBundleSetup::FragmentShader(int32 progIndex) const {
    return this->programEntries[progIndex].fragmentShader;
}

//------------------------------------------------------------------------------
const String&
ProgramBundleSetup::VertexShaderSource(int32 progIndex, ShaderLang::Code slang) const {
    return this->programEntries[progIndex].vsSources[slang];
}

//------------------------------------------------------------------------------
const String&
ProgramBundleSetup::FragmentShaderSource(int32 progIndex, ShaderLang::Code slang) const {
    return this->programEntries[progIndex].fsSources[slang];
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::VertexShaderByteCode(int32 progIndex, ShaderLang::Code slang, const void*& outPtr, uint32& outSize) const {
    outPtr = this->programEntries[progIndex].vsByteCode[slang].ptr;
    outSize = this->programEntries[progIndex].vsByteCode[slang].size;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::FragmentShaderByteCode(int32 progIndex, ShaderLang::Code slang, const void*& outPtr, uint32& outSize) const {
    outPtr = this->programEntries[progIndex].fsByteCode[slang].ptr;
    outSize = this->programEntries[progIndex].fsByteCode[slang].size;
}

//------------------------------------------------------------------------------
int32
ProgramBundleSetup::NumUniformBlocks() const {
    return this->numUniformBlockEntries;
}

//------------------------------------------------------------------------------
const StringAtom&
ProgramBundleSetup::UniformBlockName(int32 uniformBlockIndex) const {
    return this->uniformBlockEntries[uniformBlockIndex].name;
}

//------------------------------------------------------------------------------
const UniformLayout&
ProgramBundleSetup::UniformBlockLayout(int32 uniformBlockIndex) const {
    return this->uniformBlockEntries[uniformBlockIndex].layout;
}

//------------------------------------------------------------------------------
int16
ProgramBundleSetup::UniformBlockSlot(int32 uniformBlockIndex) const {
    return this->uniformBlockEntries[uniformBlockIndex].slotIndex;
}

} // namespace Oryol
