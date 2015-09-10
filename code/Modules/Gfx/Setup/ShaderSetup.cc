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
numPrograms(0),
numUniformBlocks(0),
numTextureBlocks(0) {
    // empty
}

//------------------------------------------------------------------------------
ShaderSetup::ShaderSetup(const class Locator& locator) :
Locator(locator),
numPrograms(0),
numUniformBlocks(0),
numTextureBlocks(0) {
    // empty
}

//------------------------------------------------------------------------------
ShaderSetup::programEntry&
ShaderSetup::obtainEntry(uint32 mask) {
    // find existing entry with matching mask
    for (int32 i = 0; i < this->numPrograms; i++) {
        if (this->programs[i].mask == mask) {
            return this->programs[i];
        }
    }
    // fallthrough: return new entry
    programEntry& newEntry = this->programs[this->numPrograms++];
    newEntry.mask = mask;
    return newEntry;
}

//------------------------------------------------------------------------------
void
ShaderSetup::AddProgramFromSources(uint32 mask, ShaderLang::Code slang, const VertexLayout& vsInputLayout, const String& vsSource, const String& fsSource) {
    o_assert_dbg(vsSource.IsValid() && fsSource.IsValid());

    programEntry& entry = this->obtainEntry(mask);
    entry.vsSources[slang] = vsSource;
    entry.fsSources[slang] = fsSource;
    entry.vsInputLayout = vsInputLayout;
}

//------------------------------------------------------------------------------
void
ShaderSetup::AddProgramFromByteCode(uint32 mask, ShaderLang::Code slang, const VertexLayout& vsInputLayout, const uint8* vsByteCode, uint32 vsNumBytes, const uint8* fsByteCode, uint32 fsNumBytes) {
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
ShaderSetup::AddUniformBlock(const StringAtom& name, const class UniformBlockLayout& layout, ShaderStage::Code bindStage, int32 bindSlot) {
    o_assert_dbg(name.IsValid());
    o_assert_dbg(!layout.Empty());
    o_assert_dbg(0 != layout.TypeHash);
    o_assert_dbg(bindSlot >= 0);

    uniformBlockEntry& entry = this->uniformBlocks[this->numUniformBlocks++];
    entry.name = name;
    entry.layout = layout;
    entry.bindStage = bindStage;
    entry.bindSlot = bindSlot;
}

//------------------------------------------------------------------------------
void
ShaderSetup::AddTextureBlock(const StringAtom& name, const class TextureBlockLayout& layout, ShaderStage::Code bindStage, int32 bindSlot) {
    o_assert_dbg(name.IsValid());
    o_assert_dbg(bindSlot >= 0);

    textureBlockEntry& entry = this->textureBlocks[this->numTextureBlocks++];
    entry.name = name;
    entry.layout = layout;
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
    return this->numPrograms;
}

//------------------------------------------------------------------------------
uint32
ShaderSetup::Mask(int32 progIndex) const {
    return this->programs[progIndex].mask;
}

//------------------------------------------------------------------------------
const VertexLayout&
ShaderSetup::VertexShaderInputLayout(int32 progIndex) const {
    return this->programs[progIndex].vsInputLayout;
}

//------------------------------------------------------------------------------
const String&
ShaderSetup::VertexShaderSource(int32 progIndex, ShaderLang::Code slang) const {
    return this->programs[progIndex].vsSources[slang];
}

//------------------------------------------------------------------------------
const String&
ShaderSetup::FragmentShaderSource(int32 progIndex, ShaderLang::Code slang) const {
    return this->programs[progIndex].fsSources[slang];
}

//------------------------------------------------------------------------------
void
ShaderSetup::VertexShaderByteCode(int32 progIndex, ShaderLang::Code slang, const void*& outPtr, uint32& outSize) const {
    outPtr = this->programs[progIndex].vsByteCode[slang].ptr;
    outSize = this->programs[progIndex].vsByteCode[slang].size;
}

//------------------------------------------------------------------------------
void
ShaderSetup::FragmentShaderByteCode(int32 progIndex, ShaderLang::Code slang, const void*& outPtr, uint32& outSize) const {
    outPtr = this->programs[progIndex].fsByteCode[slang].ptr;
    outSize = this->programs[progIndex].fsByteCode[slang].size;
}

//------------------------------------------------------------------------------
const String&
ShaderSetup::VertexShaderFunc(int32 progIndex, ShaderLang::Code slang) const {
    o_assert_dbg(ShaderLang::Metal == slang);
    return this->programs[progIndex].vsFuncs[slang];
}

//------------------------------------------------------------------------------
const String&
ShaderSetup::FragmentShaderFunc(int32 progIndex, ShaderLang::Code slang) const {
    o_assert_dbg(ShaderLang::Metal == slang);
    return this->programs[progIndex].fsFuncs[slang];
}

//------------------------------------------------------------------------------
int32
ShaderSetup::NumUniformBlocks() const {
    return this->numUniformBlocks;
}

//------------------------------------------------------------------------------
int32
ShaderSetup::UniformBlockIndexByStageAndSlot(ShaderStage::Code bindStage, int32 bindSlot) const {
    for (int i = 0; i < this->numUniformBlocks; i++) {
        const auto& entry = this->uniformBlocks[i];
        if ((entry.bindStage == bindStage) && (entry.bindSlot == bindSlot)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
const StringAtom&
ShaderSetup::UniformBlockName(int32 index) const {
    return this->uniformBlocks[index].name;
}

//------------------------------------------------------------------------------
const UniformBlockLayout&
ShaderSetup::UniformBlockLayout(int32 index) const {
    return this->uniformBlocks[index].layout;
}

//------------------------------------------------------------------------------
int32
ShaderSetup::UniformBlockBindSlot(int32 index) const {
    return this->uniformBlocks[index].bindSlot;
}

//------------------------------------------------------------------------------
ShaderStage::Code
ShaderSetup::UniformBlockBindStage(int32 index) const {
    return this->uniformBlocks[index].bindStage;
}

//------------------------------------------------------------------------------
int32
ShaderSetup::NumTextureBlocks() const {
    return this->numTextureBlocks;
}

//------------------------------------------------------------------------------
int32
ShaderSetup::TextureBlockIndexByStageAndSlot(ShaderStage::Code bindStage, int32 bindSlot) const {
    for (int i = 0; i < this->numTextureBlocks; i++) {
        const auto& entry = this->textureBlocks[i];
        if ((entry.bindStage == bindStage) && (entry.bindSlot == bindSlot)) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
const StringAtom&
ShaderSetup::TextureBlockName(int32 index) const {
    return this->textureBlocks[index].name;
}

//------------------------------------------------------------------------------
const TextureBlockLayout&
ShaderSetup::TextureBlockLayout(int32 index) const {
    return this->textureBlocks[index].layout;
}

//------------------------------------------------------------------------------
ShaderStage::Code
ShaderSetup::TextureBlockBindStage(int32 index) const {
    return this->textureBlocks[index].bindStage;
}

//------------------------------------------------------------------------------
int32
ShaderSetup::TextureBlockBindSlot(int32 index) const {
    return this->textureBlocks[index].bindSlot;
}

} // namespace Oryol
