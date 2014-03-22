#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::ShaderType
    @brief shader types (vertex shader, fragment shader)
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
    
class ShaderType {
public:
    /// shader types enum
    enum Code {
        VertexShader,       ///< vertex shader
        FragmentShader,     ///< fragment shader
        
        NumShaderTypes,     ///< number of shader types
        InvalidShaderType,  ///< invalid shader type
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol