#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ProgramBundleSetup
    @ingroup Gfx
    @brief setup information for a shader program bundle
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
    
class ProgramBundleSetup {
public:
    /// default constructor
    ProgramBundleSetup();
    /// construct with resource locator
    ProgramBundleSetup(const Locator& loc);

    /// the resource locator
    class Locator Locator;
    
    /// add a program consisting of precompiled vertex and fragment shader
    void AddProgram(uint32 mask, const Id& vertexShader, const Id& fragmentShader);
    /// add a program from vertex- and fragment-shader sources
    void AddProgramFromSources(uint32 mask, ShaderLang::Code slang, const String& vsSource, const String& fsSource);
    /// add a program from precompiled shader byte code
    void AddProgramFromByteCode(uint32 mask, ShaderLang::Code slang, const uint8* vsByteCode, uint32 vsNumBytes, const uint8* fsByteCode, uint32 fsNumBytes);
    /// bind a shader uniform name to a variable slot
    void AddUniform(const String& uniformName, int16 slotIndex);
    /// bind a shader uniform name to a texture variable slot
    void AddTextureUniform(const String& uniformName, int16 slotIndex);
    
    /// get number of programs
    int32 NumPrograms() const;
    /// get program mask by index
    uint32 Mask(int32 progIndex) const;
    /// get program vertex shader (only valid if setup from precompiled shaders)
    const Id& VertexShader(int32 progIndex) const;
    /// get program fragment shader (only valid if setup from precompiled shaders)
    const Id& FragmentShader(int32 progIndex) const;
    /// get program vertex shader source (only valid if setup from sources)
    const String& VertexShaderSource(int32 progIndex, ShaderLang::Code slang) const;
    /// get program fragment shader source (only valid if setup from sources)
    const String& FragmentShaderSource(int32 progIndex, ShaderLang::Code slang) const;
    /// get program vertex shader byte code, returns nullptr if no byte code exists
    void VertexShaderByteCode(int32 progIndex, ShaderLang::Code slang, const void*& outPtr, uint32& outSize) const;
    /// get program fragment shader byte code, returns nullptr if no byte code exists
    void FragmentShaderByteCode(int32 progIndex, ShaderLang::Code slang, const void*& outPtr, uint32& outSize) const;
    
    /// get number of uniforms
    int32 NumUniforms() const;
    /// get uniform name at index
    const String& UniformName(int32 uniformIndex) const;
    /// return true if uniform is a texture
    bool IsTextureUniform(int32 uniformIndex) const;
    /// get uniform slot index
    int16 UniformSlot(int32 uniformIndex) const;
    
private:
    static const int32 MaxNumProgramEntries = 8;
    static const int32 MaxNumUniformEntries = 16;

    struct programEntry {
        uint32 mask = 0;
        Id vertexShader;
        Id fragmentShader;
        String vsSources[ShaderLang::NumShaderLangs];
        String fsSources[ShaderLang::NumShaderLangs];
        struct byteCodeEntry {
            const void* ptr = nullptr;
            uint32 size = 0;
        };
        byteCodeEntry vsByteCode[ShaderLang::NumShaderLangs];
        byteCodeEntry fsByteCode[ShaderLang::NumShaderLangs];
    };

    /// obtain an existing entry with matching mask or new entry
    programEntry& obtainEntry(uint32 mask);
    
    struct uniformEntry {
        uniformEntry() : isTexture(false), slotIndex(InvalidIndex) {};
        String uniformName;
        bool isTexture;
        int16 slotIndex;
    };
    
    int32 numProgramEntries;
    int32 numUniformEntries;
    programEntry programEntries[MaxNumProgramEntries];
    uniformEntry uniformEntries[MaxNumUniformEntries];
};
    
} // namespace Oryol
 
 