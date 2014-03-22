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
ShaderSetup
ShaderSetup::FromSource(const Locator& loc, ShaderType::Code t, const String& src) {
    ShaderSetup setup;
    setup.loc = loc;
    setup.type = t;
    setup.source = src;
    return setup;
}

//------------------------------------------------------------------------------
ShaderSetup
ShaderSetup::FromSourceAndDefines(const Locator& loc, ShaderType::Code t, const String& src, const Map<String,String>& defines) {
    ShaderSetup setup;
    setup.loc = loc;
    setup.type = t;
    setup.source = src;
    return setup;
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
const String&
ShaderSetup::GetSource() const {
    return this->source;
}

//------------------------------------------------------------------------------
const Map<String,String>&
ShaderSetup::GetDefines() const {
    return this->defines;
}

} // namespace Render
} // namespace Oryol