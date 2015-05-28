//------------------------------------------------------------------------------
//  ShaderSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ShaderSetup.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
ShaderSetup::ShaderSetup() :
Type(ShaderType::InvalidShaderType) {
    // empty
}

//------------------------------------------------------------------------------
ShaderSetup::ShaderSetup(const class Locator& l, ShaderType::Code t) :
Locator(l),
Type(t) {
    // empty
}

//------------------------------------------------------------------------------
void
ShaderSetup::AddSource(ShaderLang::Code slang, const String& source) {
    o_assert_range(slang, ShaderLang::NumShaderLangs);
    this->sources[slang] = source;
}

//------------------------------------------------------------------------------
const String&
ShaderSetup::Source(ShaderLang::Code slang) const {
    o_assert_range(slang, ShaderLang::NumShaderLangs);
    o_assert_dbg(this->sources[slang].IsValid());
    return this->sources[slang];
}

//------------------------------------------------------------------------------
void
ShaderSetup::AddByteCode(ShaderLang::Code slang, const void* ptr, uint32 byteSize) {
    o_assert_range(slang, ShaderLang::NumShaderLangs);
    o_assert_dbg(ptr);
    o_assert_dbg(byteSize > 0);
    this->byteCode[slang].ptr = ptr;
    this->byteCode[slang].size = byteSize;
}

//------------------------------------------------------------------------------
void
ShaderSetup::ByteCode(ShaderLang::Code slang, const void*& outPtr, uint32& outByteSize) const {
    o_assert_range(slang, ShaderLang::NumShaderLangs);
    o_assert_dbg(nullptr != this->byteCode[slang].ptr);
    outPtr = this->byteCode[slang].ptr;
    outByteSize = this->byteCode[slang].size;
}

} // namespace Oryol