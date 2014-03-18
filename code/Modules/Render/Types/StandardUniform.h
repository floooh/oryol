#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::StandardUniform
    @brief standard shader uniform variables
    
    StandardUniform are shader uniforms which are updated automatically
    by the rendering system. Since shaders have no reflection information,
    standard uniforms must be manually bound shader Program is created. Once
    bound the rendering system will update the uniform values when needed.
    
    The most important StandardUniform is the ModelViewProj matrix, but
    there are also a couple of other useful matrices, and a continouous
    Timer for implementing simple animations in shaders.
    
    @see Program, ProgramSetup
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
    
class StandardUniform {
public:
    /// standard variables enum
    enum Code {
        ModelViewProj,
        Time,
        // more to come...
        
        NumStandardUniforms,
        InvalidStandardUniform,
    };
    
    /// convert to string (FIXME: should this also be the required shader uniform name?)
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol
 