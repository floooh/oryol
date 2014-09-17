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
#include "Gfx/Core/GfxId.h"

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
    void AddProgram(uint32 mask, const GfxId& vertexShader, const GfxId& fragmentShader);
    /// add a program from vertex- and fragment-shader sources
    void AddProgramFromSources(uint32 mask, ShaderLang::Code slang, const String& vsSource, const String& fsSource);
    /// bind a shader uniform name to a variable slot
    void AddUniform(const String& uniformName, int16 slotIndex);
    /// bind a shader uniform name to a texture variable slot
    void AddTextureUniform(const String& uniformName, int16 slotIndex);
    
    /// get number of programs
    int32 NumPrograms() const;
    /// get program mask by index
    uint32 Mask(int32 progIndex) const;
    /// get program vertex shader (only valid if setup from precompiled shaders)
    const GfxId& VertexShader(int32 progIndex) const;
    /// get program fragment shader (only valid if setup from precompiled shaders)
    const GfxId& FragmentShader(int32 progIndex) const;
    /// get program vertex shader source (only valid if setup from sources)
    const String& VertexShaderSource(int32 progIndex, ShaderLang::Code slang) const;
    /// get program fragment shader source (only valid if setup from sources)
    const String& FragmentShaderSource(int32 progIndex, ShaderLang::Code slang) const;
    
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
        programEntry() : mask(0) {};
        uint32 mask;
        GfxId vertexShader;
        GfxId fragmentShader;
        String vsSources[ShaderLang::NumShaderLangs];
        String fsSources[ShaderLang::NumShaderLangs];
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
 
 