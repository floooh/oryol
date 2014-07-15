//------------------------------------------------------------------------------
//  OpenMode.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <cstring>
#include "OpenMode.h"
#include "Core/Macros.h"
#include "Core/Assert.h"

namespace Oryol {
namespace IO {

//------------------------------------------------------------------------------
const char* OpenMode::ToString(OpenMode::Enum m) {
    switch (m) {
        __ORYOL_TOSTRING(ReadOnly);
        __ORYOL_TOSTRING(WriteOnly);
        __ORYOL_TOSTRING(WriteAppend);
        __ORYOL_TOSTRING(ReadWrite);
        __ORYOL_TOSTRING(ReadWriteAppend);
        default: return "InvalidOpenMode";
    }
}

//------------------------------------------------------------------------------
OpenMode::Enum OpenMode::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(ReadOnly);
    __ORYOL_FROMSTRING(WriteOnly);
    __ORYOL_FROMSTRING(WriteAppend);
    __ORYOL_FROMSTRING(ReadWrite);
    __ORYOL_FROMSTRING(ReadWriteAppend);
    return OpenMode::InvalidOpenMode;
}

} // namespace IO
} // namespace Oryol