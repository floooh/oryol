#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::ProgramBundleSetup
    @brief setup information for a shader program bundle
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class ProgramBundleSetup {
public:
    /// default constructor
    ProgramBundleSetup();
    /// construct with resource locator
    ProgramBundleSetup(const Resource::Locator& loc);

    /// the resource locator
    class Resource::Locator Locator;
    
    /// add a program consisting of precompiled vertex and fragment shader
    void AddProgram(uint32 mask, const Resource::Id& vertexShader, const Resource::Id& fragmentShader);
    /// add a program from vertex- and fragment-shader sources
    void AddProgramFromSources(uint32 mask, ShaderLang::Code slang, const Core::String& vsSource, const Core::String& fsSource);
    /// bind a shader uniform name to a variable slot
    void AddUniform(const Core::String& uniformName, int16 slotIndex);
    /// bind a shader uniform name to a texture variable slot
    void AddTextureUniform(const Core::String& uniformName, int16 slotIndex);
    
    /// get number of programs
    int32 NumPrograms() const;
    /// get program mask by index
    uint32 Mask(int32 progIndex) const;
    /// get program vertex shader (only valid if setup from precompiled shaders)
    const Resource::Id& VertexShader(int32 progIndex) const;
    /// get program fragment shader (only valid if setup from precompiled shaders)
    const Resource::Id& FragmentShader(int32 progIndex) const;
    /// get program vertex shader source (only valid if setup from sources)
    const Core::String& VertexShaderSource(int32 progIndex, ShaderLang::Code slang) const;
    /// get program fragment shader source (only valid if setup from sources)
    const Core::String& FragmentShaderSource(int32 progIndex, ShaderLang::Code slang) const;
    
    /// get number of uniforms
    int32 NumUniforms() const;
    /// get uniform name at index
    const Core::String& UniformName(int32 uniformIndex) const;
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
        Resource::Id vertexShader;
        Resource::Id fragmentShader;
        Core::String vsSources[ShaderLang::NumShaderLangs];
        Core::String fsSources[ShaderLang::NumShaderLangs];
    };

    /// obtain an existing entry with matching mask or new entry
    programEntry& obtainEntry(uint32 mask);
    
    struct uniformEntry {
        uniformEntry() : isTexture(false), slotIndex(InvalidIndex) {};
        Core::String uniformName;
        bool isTexture;
        int16 slotIndex;
    };
    
    int32 numProgramEntries;
    int32 numUniformEntries;
    programEntry programEntries[MaxNumProgramEntries];
    uniformEntry uniformEntries[MaxNumUniformEntries];
};
    
} // namespace Render
} // namespace Oryol
 
 