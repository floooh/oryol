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
type(ShaderType::InvalidShaderType) {
    // empty
}

//------------------------------------------------------------------------------
ShaderSetup::ShaderSetup(const Locator& l, ShaderType::Code t) :
loc(l),
type(t) {
    // empty
}

//------------------------------------------------------------------------------
const Locator&
ShaderSetup::GetLocator() const {
    return this->loc;
}

//------------------------------------------------------------------------------
ShaderType::Code
ShaderSetup::GetType() const {
    return this->type;
}

//------------------------------------------------------------------------------
void
ShaderSetup::AddSource(ShaderLang::Code slang, const String& source) {
    o_assert_range(slang, ShaderLang::NumShaderLangs);
    this->sources[slang] = source;
}

//------------------------------------------------------------------------------
const String&
ShaderSetup::GetSource(ShaderLang::Code slang) const {
    o_assert_range(slang, ShaderLang::NumShaderLangs);
    return this->sources[slang];
}

} // namespace Render
} // namespace Oryol