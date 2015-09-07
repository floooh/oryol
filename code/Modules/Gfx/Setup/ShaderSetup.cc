//------------------------------------------------------------------------------
//  ShaderSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ShaderSetup.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {

//------------------------------------------------------------------------------
ShaderSetup::ShaderSetup() :
Locator(Locator::NonShared()),
libraryByteCodeSize(0),
libraryByteCode(nullptr),
numProgramEntries(0),
numUniformBlockEntries(0),
numTextureEntries(0) {
    // empty
}

//------------------------------------------------------------------------------
ShaderSetup::ShaderSetup(const class Locator& locator) :
Locator(locator),
numProgramEntries(0),
numUniformBlockEntries(0),
numTextureEntries(0) {
    // empty
}

//------------------------------------------------------------------------------
ShaderSetup::programEntry&
ShaderSetup::obtainEntry(uint32 mask) {
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
ShaderSetup::AddProgramFromSources(uint32 mask, ShaderLang::Code slang, const VertexLayout& vsInputLayout, const String& vsSource, const String& fsSource) {
    o_assert_dbg(this->numProgramEntries < GfxConfig::MaxNumBundlePrograms);
    o_assert_dbg(vsSource.IsValid() && fsSource.IsValid());

    programEntry& entry = this->obtainEntry(mask);
    entry.vsSources[slang] = vsSource;
    entry.fsSources[slang] = fsSource;
    entry.vsInputLayout = vsInputLayout;
}

//------------------------------------------------------------------------------
void
ShaderSetup::AddProgramFromByteCode(uint32 mask, ShaderLang::Code slang, const VertexLayout& vsInputLayout, const uint8* vsByteCode, uint32 vsNumBytes, const uint8* fsByteCode, uint32 fsNumBytes) {
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
ShaderSetup::AddProgramFromLibrary(uint32 mask, ShaderLang::Code slang, const Oryol::VertexLayout &vsInputLayout, const char *vsFunc, const char *fsFunc) {
    o_assert_dbg(ShaderLang::Metal == slang);
    o_assert_dbg(vsFunc && fsFunc);
    programEntry& entry = this->obtainEntry(mask);
    entry.vsFuncs[slang] = vsFunc;
    entry.fsFuncs[slang] = fsFunc;
    entry.vsInputLayout = vsInputLayout;
}

//------------------------------------------------------------------------------
void
ShaderSetup::AddUniformBlock(const StringAtom& name, const UniformLayout& layout, ShaderStage::Code bindStage, int32 bindSlot) {
    o_assert_dbg(name.IsValid());
    o_assert_dbg(!layout.Empty());
    o_assert_dbg(0 != layout.TypeHash);
    o_assert_dbg(bindSlot >= 0);

    uniformBlockEntry& entry = this->uniformBlockEntries[this->numUniformBlockEntries++];
    entry.name = name;
    entry.layout = layout;
    entry.bindStage = bindStage;
    entry.bindSlot = bindSlot;
}

//------------------------------------------------------------------------------
void
ShaderSetup::AddTexture(const StringAtom& name, TextureType::Code texType, ShaderStage::Code bindStage, int32 bindSlot) {
    o_assert_dbg(name.IsValid());
    o_assert_dbg(bindSlot >= 0);

    textureEntry& entry = this->textureEntries[this->numTextureEntries++];
    entry.name = name;
    entry.type = texType;
    entry.bindStage = bindStage;
    entry.bindSlot = bindSlot;
}

//------------------------------------------------------------------------------
void
ShaderSetup::SetLibraryByteCode(ShaderLang::Code slang, const uint8* byteCode, uint32 numBytes) {
    o_assert_dbg(ShaderLang::Metal == slang);
    o_assert_dbg(nullptr != byteCode);
    o_assert_dbg(numBytes > 0);
    this->libraryByteCode = byteCode;
    this->libraryByteCodeSize = numBytes;
}

//------------------------------------------------------------------------------
void
ShaderSetup::LibraryByteCode(ShaderLang::Code slang, const void *&outPtr, uint32 &outSize) const {
    o_assert_dbg(ShaderLang::Metal == slang);
    outPtr = this->libraryByteCode;
    outSize = this->libraryByteCodeSize;
}

//------------------------------------------------------------------------------
int32
ShaderSetup::NumPrograms() const {
    return this->numProgramEntries;
}

//------------------------------------------------------------------------------
uint32
ShaderSetup::Mask(int32 progIndex) const {
    return this->programEntries[progIndex].mask;
}

//------------------------------------------------------------------------------
const VertexLayout&
ShaderSetup::VertexShaderInputLayout(int32 progIndex) const {
    return this->programEntries[progIndex].vsInputLayout;
}

//------------------------------------------------------------------------------
const String&
ShaderSetup::VertexShaderSource(int32 progIndex, ShaderLang::Code slang) const {
    return this->programEntries[progIndex].vsSources[slang];
}

//------------------------------------------------------------------------------
const String&
ShaderSetup::FragmentShaderSource(int32 progIndex, ShaderLang::Code slang) const {
    return this->programEntries[progIndex].fsSources[slang];
}

//------------------------------------------------------------------------------
void
ShaderSetup::VertexShaderByteCode(int32 progIndex, ShaderLang::Code slang, const void*& outPtr, uint32& outSize) const {
    outPtr = this->programEntries[progIndex].vsByteCode[slang].ptr;
    outSize = this->programEntries[progIndex].vsByteCode[slang].size;
}

//------------------------------------------------------------------------------
void
ShaderSetup::FragmentShaderByteCode(int32 progIndex, ShaderLang::Code slang, const void*& outPtr, uint32& outSize) const {
    outPtr = this->programEntries[progIndex].fsByteCode[slang].ptr;
    outSize = this->programEntries[progIndex].fsByteCode[slang].size;
}

//------------------------------------------------------------------------------
const String&
ShaderSetup::VertexShaderFunc(int32 progIndex, ShaderLang::Code slang) const {
    o_assert_dbg(ShaderLang::Metal == slang);
    return this->programEntries[progIndex].vsFuncs[slang];
}

//------------------------------------------------------------------------------
const String&
ShaderSetup::FragmentShaderFunc(int32 progIndex, ShaderLang::Code slang) const {
    o_assert_dbg(ShaderLang::Metal == slang);
    return this->programEntries[progIndex].fsFuncs[slang];
}

//------------------------------------------------------------------------------
int32
ShaderSetup::NumUniformBlocks() const {
    return this->numUniformBlockEntries;
}

//------------------------------------------------------------------------------
const StringAtom&
ShaderSetup::UniformBlockName(int32 index) const {
    return this->uniformBlockEntries[index].name;
}

//------------------------------------------------------------------------------
const UniformLayout&
ShaderSetup::UniformBlockLayout(int32 index) const {
    return this->uniformBlockEntries[index].layout;
}

//------------------------------------------------------------------------------
int32
ShaderSetup::UniformBlockBindSlot(int32 index) const {
    return this->uniformBlockEntries[index].bindSlot;
}

//------------------------------------------------------------------------------
ShaderStage::Code
ShaderSetup::UniformBlockBindStage(int32 index) const {
    return this->uniformBlockEntries[index].bindStage;
}

//------------------------------------------------------------------------------
const UniformLayout*
ShaderSetup::FindUniformBlockLayout(ShaderStage::Code bindStage, int32 bindSlot) const {
    for (int i = 0; i < this->numUniformBlockEntries; i++) {
        const auto& entry = this->uniformBlockEntries[i];
        if ((entry.bindStage == bindStage) && (entry.bindSlot == bindSlot)) {
            return &entry.layout;
        }
    }
    return nullptr;
}

//------------------------------------------------------------------------------
int32
ShaderSetup::NumTextures() const {
    return this->numTextureEntries;
}

//------------------------------------------------------------------------------
const StringAtom&
ShaderSetup::TextureName(int32 index) const {
    return this->textureEntries[index].name;
}

//------------------------------------------------------------------------------
TextureType::Code
ShaderSetup::TextureType(int32 index) const {
    return this->textureEntries[index].type;
}

//------------------------------------------------------------------------------
ShaderStage::Code
ShaderSetup::TextureBindStage(int32 index) const {
    return this->textureEntries[index].bindStage;
    
}

//------------------------------------------------------------------------------
int32
ShaderSetup::TextureBindSlot(int32 index) const {
    return this->textureEntries[index].bindSlot;
}

} // namespace Oryol
