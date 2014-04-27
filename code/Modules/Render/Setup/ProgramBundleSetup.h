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

namespace Oryol {
namespace Render {
    
class ProgramBundleSetup {
public:
    /// default constructor
    ProgramBundleSetup();
    /// construct with resource locator
    ProgramBundleSetup(const Resource::Locator& loc);
    
    /// add a program consisting of precompiled vertex and fragment shader
    void AddProgram(uint32 mask, const Resource::Id& vertexShader, const Resource::Id& fragmentShader);
    /// add a program from vertex- and fragment-shader sources
    void AddProgramFromSources(uint32 mask, const Core::String& vsSource, const Core::String& fsSource);
    /// bind a shader uniform name to a variable slot
    void AddUniform(const Core::String& uniformName, int16 slotIndex);
    /// bind a shader uniform name to a texture variable slot
    void AddTextureUniform(const Core::String& uniformName, int16 slotIndex);
    
    /// get the resource locator
    const Resource::Locator& GetLocator() const;
    
    /// get number of programs
    int32 GetNumPrograms() const;
    /// get program mask by index
    uint32 GetMask(int32 progIndex) const;
    /// get program vertex shader (only valid if setup from precompiled shaders)
    const Resource::Id& GetVertexShader(int32 progIndex) const;
    /// get program fragment shader (only valid if setup from precompiled shaders)
    const Resource::Id& GetFragmentShader(int32 progIndex) const;
    /// get program vertex shader source (only valid if setup from sources)
    const Core::String& GetVertexShaderSource(int32 progIndex) const;
    /// get program fragment shader source (only valid if setup from sources)
    const Core::String& GetFragmentShaderSource(int32 progIndex) const;
    
    /// get number of uniforms
    int32 GetNumUniforms() const;
    /// get uniform name at index
    const Core::String& GetUniformName(int32 uniformIndex) const;
    /// return true if uniform is a texture
    bool IsTextureUniform(int32 uniformIndex) const;
    /// get uniform slot index
    int16 GetUniformSlot(int32 uniformIndex) const;
    
private:
    static const int32 MaxNumProgramEntries = 8;
    static const int32 MaxNumUniformEntries = 16;

    struct programEntry {
        programEntry() : mask(0) {};
        uint32 mask;
        Resource::Id vertexShader;
        Resource::Id fragmentShader;
        Core::String vsSource;
        Core::String fsSource;
    };
    struct uniformEntry {
        uniformEntry() : isTexture(false), slotIndex(InvalidIndex) {};
        Core::String uniformName;
        bool isTexture;
        int16 slotIndex;
    };
    
    Resource::Locator loc;
    int32 numProgramEntries;
    int32 numUniformEntries;
    programEntry programEntries[MaxNumProgramEntries];
    uniformEntry uniformEntries[MaxNumUniformEntries];
};
    
} // namespace Render
} // namespace Oryol
 
 