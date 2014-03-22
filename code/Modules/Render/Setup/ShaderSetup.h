#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::ShaderSetup
    @brief setup class for vertex- or fragment shaders
 */
#include "Core/Containers/Map.h"
#include "Core/String/String.h"
#include "Render/Types/ShaderType.h"
#include "Resource/Locator.h"

namespace Oryol {
namespace Render {
    
class ShaderSetup {
public:
    /// setup from source string
    static ShaderSetup FromSource(const Resource::Locator& loc, ShaderType::Code type, const Core::String& source);
    /// setup from source string and defines
    static ShaderSetup FromSourceAndDefines(const Resource::Locator& loc, ShaderType::Code type, const Core::String& source, const Core::Map<Core::String,Core::String>& defines);
    
    /// default constructor
    ShaderSetup();
    /// get locator
    const Resource::Locator& GetLocator() const;
    /// get the shader type
    ShaderType::Code GetType() const;
    /// get the shader source
    const Core::String& GetSource() const;
    /// get defines
    const Core::Map<Core::String,Core::String>& GetDefines() const;
    
private:
    Resource::Locator loc;
    ShaderType::Code type;
    Core::String source;
    Core::Map<Core::String,Core::String> defines;
};
    
} // namespace Render
} // namespace Oryol
