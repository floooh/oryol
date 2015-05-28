#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderSetup
    @ingroup Gfx
    @brief setup class for vertex- or fragment shaders
 */
#include "Core/Containers/Map.h"
#include "Core/String/String.h"
#include "Gfx/Core/Enums.h"
#include "Resource/Locator.h"

namespace Oryol {
    
class ShaderSetup {
public:
    /// default constructor
    ShaderSetup();
    /// construct with locator and shader type
    ShaderSetup(const Locator& loc, ShaderType::Code type);
    /// resource locator
    class Locator Locator;
    /// shader type
    ShaderType::Code Type;
    /// add shader source code of a shader language syntax
    void AddSource(ShaderLang::Code slang, const String& source);
    /// get the shader sources by shader language
    const String& Source(ShaderLang::Code slang) const;
    /// add shader byte code (data must be static and remain at position in memory)
    void AddByteCode(ShaderLang::Code slang, const void* ptr, uint32 byteSize);
    /// get shader byte code by shader language, outPtr will be nullptr if no byte code exists
    void ByteCode(ShaderLang::Code slang, const void*& outPtr, uint32& outByteSize) const;
    
private:
    String sources[ShaderLang::NumShaderLangs];
    struct byteCodeEntry {
        const void* ptr = nullptr;
        uint32 size = 0;
    } byteCode[ShaderLang::NumShaderLangs];
};
    
} // namespace Oryol
