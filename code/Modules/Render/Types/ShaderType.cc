//------------------------------------------------------------------------------
//  ShaderType.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ShaderType.h"
#include "Core/Assert.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
ShaderType::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(VertexShader);
        __ORYOL_TOSTRING(FragmentShader);
        default: return "InvalidShaderType";
    }
}

//------------------------------------------------------------------------------
ShaderType::Code
ShaderType::FromString(const char* str) {
    __ORYOL_FROMSTRING(VertexShader);
    __ORYOL_FROMSTRING(FragmentShader);
    return ShaderType::InvalidShaderType;
}

} // namespace Render
} // namespace Oryol