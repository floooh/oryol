//------------------------------------------------------------------------------
//  ResourceType.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ResourceType.h"
#include "Core/Assert.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
const char*
ResourceType::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Texture);
        __ORYOL_TOSTRING(Mesh);
        __ORYOL_TOSTRING(Shader);
        __ORYOL_TOSTRING(ProgramBundle);
        __ORYOL_TOSTRING(StateBlock);
        __ORYOL_TOSTRING(ConstantBlock);
        default: return "InvalidResourceType";
    }
}

//------------------------------------------------------------------------------
ResourceType::Code
ResourceType::FromString(const char* str) {
    __ORYOL_FROMSTRING(Texture);
    __ORYOL_FROMSTRING(Mesh);
    __ORYOL_FROMSTRING(Shader);
    __ORYOL_FROMSTRING(ProgramBundle);
    __ORYOL_FROMSTRING(StateBlock);
    __ORYOL_FROMSTRING(ConstantBlock);
    return InvalidResourceType;
}

} // namespace Render
} // namespace Oryol