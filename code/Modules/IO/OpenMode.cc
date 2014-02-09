//------------------------------------------------------------------------------
//  OpenMode.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <cstring>
#include "OpenMode.h"

namespace Oryol {
namespace IO {

//------------------------------------------------------------------------------
const char* OpenMode::ToString(OpenMode::Enum m) {
    switch (m) {
        case OpenMode::ReadOnly:        return "ReadOnly";
        case OpenMode::WriteOnly:       return "WriteOnly";
        case OpenMode::WriteAppend:     return "WriteAppend";
        case OpenMode::ReadWrite:       return "ReadWrite";
        case OpenMode::ReadWriteAppend: return "ReadWriteAppend";
        default:                        return "Invalid";
    }
}

//------------------------------------------------------------------------------
OpenMode::Enum OpenMode::FromString(const char* str) {
    o_assert(str);
    if (std::strcmp(str, "ReadOnly") == 0) return OpenMode::ReadOnly;
    else if (std::strcmp(str, "WriteOnly") == 0) return OpenMode::WriteOnly;
    else if (std::strcmp(str, "WriteAppend") == 0) return OpenMode::WriteAppend;
    else if (std::strcmp(str, "ReadWrite") == 0) return OpenMode::ReadWrite;
    else if (std::strcmp(str, "ReadWriteAppend") == 0) return OpenMode::ReadWriteAppend;
    else return OpenMode::Invalid;
}

} // namespace IO
} // namespace Oryol