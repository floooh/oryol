//------------------------------------------------------------------------------
//  ShaderSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ShaderSetup.h"

namespace Oryol {
namespace Render {
    
using namespace Core;
using namespace Resource;

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
    return this->sources[slang];
}

} // namespace Render
} // namespace Oryol