//------------------------------------------------------------------------------
//  baseFileAccess.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "baseFileAccess.h"
#include "Core/Assertion.h"
#include <limits>

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
HostFileHandle*
baseFileAccess::Open(const char *path, OpenMode::Enum mode) const {
    return nullptr;
}

//------------------------------------------------------------------------------
void
baseFileAccess::Close(HostFileHandle* file) const {
}

//------------------------------------------------------------------------------
int64
baseFileAccess::Write(HostFileHandle* file, const void* ptr, int64 bufferSize) const {
    return -1;
}

//------------------------------------------------------------------------------
int64
baseFileAccess::Read(HostFileHandle* file, void* ptr, int64 bufferSize) const {
    return -1;
}

//------------------------------------------------------------------------------
int64
baseFileAccess::Tell(HostFileHandle* file) const {
    return 0L;
}

//------------------------------------------------------------------------------
int64
baseFileAccess::Seek(HostFileHandle* file, int64 pos, SeekType type) const {
    return 0L;
}

//------------------------------------------------------------------------------
int64
baseFileAccess::Length(HostFileHandle* file) const {
    const int64 pos = Tell(file);
    Seek(file, 0, SeekType::End);
    const int64 end = Tell(file);
    Seek(file, pos, SeekType::Set);
    return end;
}

//------------------------------------------------------------------------------
const char*
baseFileAccess::ResolveMode(OpenMode::Enum mode) const {
    if (OpenMode::ReadOnly == mode)
        return "rb";
    else if (OpenMode::WriteOnly == mode)
        return "wb";
    else if (OpenMode::WriteAppend == mode)
        return "ab";
    else if (OpenMode::ReadWrite == mode)
        return "r+";
    else if (OpenMode::ReadWriteAppend == mode)
        return "a+";
    return nullptr;
}

} // namespace _priv
} // namespace Oryol
