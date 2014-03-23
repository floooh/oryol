#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::ProgramSetup
    @brief setup object for initialization a shader program
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Render/Types/StandardUniform.h"

namespace Oryol {
namespace Render {
    
class ProgramSetup {
public:
    /// default constructor
    ProgramSetup();
    
    /// add vertex- and fragment shader sources
    void AddCode(uint32 mask, const Core::String& vsSource, const Core::String& fsSource);
    /// bind a shader uniform name to a variable slot
    void AddUniform(const Core::String& uniformName, uint32 slotIndex);
    /// bind a shader uniform name to a standard variable
    void AddStandardUniform(const Core::String& uniformName, StandardUniform::Code stdUniform);
    
private:
    static const int32 MaxNumCodeEntries = 8;
    static const int32 MaxNumUniformEntries = 16;
    static const int32 MaxNumStdUniformEntries = 8;

    struct codeEntry {
        codeEntry() : mask(0) {};
        uint32 mask;
        Core::String vsSource;
        Core::String fsSource;
    };
    struct uniformEntry {
        uniformEntry() : slotIndex(InvalidIndex) {};
        Core::String uniformName;
        uint32 slotIndex;
    };
    struct stdUniformEntry {
        stdUniformEntry() : stdUniform(StandardUniform::InvalidStandardUniform) { };
        Core::String uniformName;
        StandardUniform::Code stdUniform;
    };
    
    int32 numCodeEntries;
    int32 numUniformEntries;
    int32 numStdUniformEntries;
    codeEntry codeEntries[MaxNumCodeEntries];
    uniformEntry uniformEntries[MaxNumUniformEntries];
    stdUniformEntry stdUniformEntries[MaxNumStdUniformEntries];
};
    
} // namespace Render
} // namespace Oryol
 
 