//------------------------------------------------------------------------------
//  StandardUniform.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "StandardUniform.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
const char*
StandardUniform::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(ModelViewProj);
        __ORYOL_TOSTRING(Time);
        default: return "InvalidStandardUniform";
    }
}

//------------------------------------------------------------------------------
StandardUniform::Code
StandardUniform::FromString(const char* str) {
    __ORYOL_FROMSTRING(ModelViewProj);
    __ORYOL_FROMSTRING(Time);
    return InvalidStandardUniform;
}

} // namespace Render
} // namespace Oryol
