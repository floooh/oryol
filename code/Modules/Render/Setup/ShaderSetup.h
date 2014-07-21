#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::ShaderSetup
    @brief setup class for vertex- or fragment shaders
 */
#include "Core/Containers/Map.h"
#include "Core/String/String.h"
#include "Render/Core/Enums.h"
#include "Resource/Locator.h"

namespace Oryol {
namespace Render {
    
class ShaderSetup {
public:
    /// default constructor
    ShaderSetup();
    /// construct with locator and shader type
    ShaderSetup(const Resource::Locator& loc, ShaderType::Code type);
    /// resource locator
    Resource::Locator Locator;
    /// shader type
    ShaderType::Code Type;
    /// add a source code of a shader language syntax
    void AddSource(ShaderLang::Code slang, const Core::String& source);
    /// get the shader sources by shader language version (may return empty string)
    const Core::String& Source(ShaderLang::Code slang) const;
    
private:
    Core::String sources[ShaderLang::NumShaderLangs];
};
    
} // namespace Render
} // namespace Oryol
