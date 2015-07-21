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
libraryByteCodeSize(0),
libraryByteCode(nullptr),
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
ProgramBundleSetup::AddProgramFromSources(uint32 mask, ShaderLang::Code slang, const VertexLayout& vsInputLayout, const String& vsSource, const String& fsSource) {
    o_assert_dbg(this->numProgramEntries < GfxConfig::MaxNumBundlePrograms);
    o_assert_dbg(vsSource.IsValid() && fsSource.IsValid());

    programEntry& entry = this->obtainEntry(mask);
    entry.vsSources[slang] = vsSource;
    entry.fsSources[slang] = fsSource;
    entry.vsInputLayout = vsInputLayout;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddProgramFromByteCode(uint32 mask, ShaderLang::Code slang, const VertexLayout& vsInputLayout, const uint8* vsByteCode, uint32 vsNumBytes, const uint8* fsByteCode, uint32 fsNumBytes) {
    o_assert_dbg(this->numProgramEntries < GfxConfig::MaxNumBundlePrograms);
    o_assert_dbg(vsByteCode && (vsNumBytes > 0));
    o_assert_dbg(fsByteCode && (fsNumBytes > 0));

    programEntry& entry = this->obtainEntry(mask);
    entry.vsByteCode[slang].ptr = vsByteCode;
    entry.vsByteCode[slang].size = vsNumBytes;
    entry.fsByteCode[slang].ptr = fsByteCode;
    entry.fsByteCode[slang].size = fsNumBytes;
    entry.vsInputLayout = vsInputLayout;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddProgramFromLibrary(uint32 mask, ShaderLang::Code slang, const Oryol::VertexLayout &vsInputLayout, const char *vsFunc, const char *fsFunc) {
    o_assert_dbg(ShaderLang::Metal == slang);
    o_assert_dbg(vsFunc && fsFunc);
    programEntry& entry = this->obtainEntry(mask);
    entry.vsFuncs[slang] = vsFunc;
    entry.fsFuncs[slang] = fsFunc;
    entry.vsInputLayout = vsInputLayout;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::AddUniformBlock(const StringAtom& name, const UniformLayout& layout, ShaderType::Code shaderStage, int32 slotIndex) {
    o_assert_dbg(name.IsValid());
    o_assert_dbg(!layout.Empty());
    o_assert_dbg(0 != layout.TypeHash);

    uniformBlockEntry& entry = this->uniformBlockEntries[this->numUniformBlockEntries++];
    entry.name = name;
    entry.layout = layout;
    entry.shaderStage = shaderStage;
    entry.slotIndex = slotIndex;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::SetLibraryByteCode(ShaderLang::Code slang, const uint8* byteCode, uint32 numBytes) {
    o_assert_dbg(ShaderLang::Metal == slang);
    o_assert_dbg(nullptr != byteCode);
    o_assert_dbg(numBytes > 0);
    this->libraryByteCode = byteCode;
    this->libraryByteCodeSize = numBytes;
}

//------------------------------------------------------------------------------
void
ProgramBundleSetup::LibraryByteCode(ShaderLang::Code slang, const void *&outPtr, uint32 &outSize) const {
    o_assert_dbg(ShaderLang::Metal == slang);
    outPtr = this->libraryByteCode;
    outSize = this->libraryByteCodeSize;
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
const VertexLayout&
ProgramBundleSetup::VertexShaderInputLayout(int32 progIndex) const {
    return this->programEntries[progIndex].vsInputLayout;
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
const String&
ProgramBundleSetup::VertexShaderFunc(int32 progIndex, ShaderLang::Code slang) const {
    o_assert_dbg(ShaderLang::Metal == slang);
    return this->programEntries[progIndex].vsFuncs[slang];
}

//------------------------------------------------------------------------------
const String&
ProgramBundleSetup::FragmentShaderFunc(int32 progIndex, ShaderLang::Code slang) const {
    o_assert_dbg(ShaderLang::Metal == slang);
    return this->programEntries[progIndex].fsFuncs[slang];
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
int32
ProgramBundleSetup::UniformBlockSlot(int32 uniformBlockIndex) const {
    return this->uniformBlockEntries[uniformBlockIndex].slotIndex;
}

//------------------------------------------------------------------------------
ShaderType::Code
ProgramBundleSetup::UniformBlockShaderStage(int32 uniformBlockIndex) const {
    return this->uniformBlockEntries[uniformBlockIndex].shaderStage;
}

} // namespace Oryol
