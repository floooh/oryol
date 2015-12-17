//------------------------------------------------------------------------------
//  winFileAccess.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "winFileAccess.h"
#include "Core/String/StringBuilder.h"
#include <limits>
#include <cstring>
#include <windows.h>
#include <fcntl.h>
#include <direct.h>
#include <io.h>

namespace Oryol {
namespace _priv {

#define FORWARD(file) reinterpret_cast<HostFileHandle*>(file)
#define BACKWARD(handle) reinterpret_cast<FILE*>(handle)

//------------------------------------------------------------------------------
HostFileHandle*
winFileAccess::Open(const char *path, OpenMode::Enum mode) const {
    const char *modeStr = ResolveMode(mode);
    if (modeStr == nullptr)
        return nullptr;
#ifdef _MSC_VER
    FILE* file = nullptr;
    errno_t e = fopen_s(&file, path, modeStr);
    if (e == 0) {
        return FORWARD(file);
    }
    return nullptr;
#else
    return FORWARD(::fopen(path, modeStr));
#endif
}

//------------------------------------------------------------------------------
void
winFileAccess::Close(HostFileHandle* file) const {
    fclose(BACKWARD(file));
}

//------------------------------------------------------------------------------
int64
winFileAccess::Tell(HostFileHandle* file) const {
    const int64 pos = ::ftell(BACKWARD(file));
    return pos;
}

//------------------------------------------------------------------------------
int64
winFileAccess::Seek(HostFileHandle* file, int64 pos, SeekType type) const {
    int mode = SEEK_SET;
    if (SeekType::End == type)
        mode = SEEK_END;
    return ::fseek(BACKWARD(file), static_cast<long>(pos), mode);
}

//------------------------------------------------------------------------------
int64
winFileAccess::Write(HostFileHandle* file, const void* ptr, int64 numBytes) const {
    if (numBytes <= 0L) {
        return 0L;
    }
    const uchar *buf = static_cast<const uchar*>(ptr);
    int64 remaining = numBytes;
    while (remaining) {
        const size_t written = ::fwrite(buf, 1, remaining, BACKWARD(file));
        if (written == 0u) {
            return -1L;
        }

        remaining -= written;
        buf += written;
    }

    return numBytes;
}

//------------------------------------------------------------------------------
int64
winFileAccess::Read(HostFileHandle* file, void* ptr, int64 bufferSize) const {
    if (file == nullptr)
        return -1;
    if (bufferSize <= 0 || ptr == nullptr)
        return 0;

    unsigned char *buf = (unsigned char *) ptr;
    const int64 len = Length(file);

    size_t remaining = bufferSize < len ? bufferSize : len;
    int64 readPosition = 0;

    Seek(file, 0, SeekType::Set);
    while (remaining) {
        const size_t readAmount = ::fread(buf, 1, remaining, BACKWARD(file));
        if (readAmount == 0) {
            if (::feof(BACKWARD(file))) {
                return readPosition;
            }
            return -1;
        }
        remaining -= readAmount;
        buf += readAmount;
        readPosition += static_cast<int64>(readAmount);
    }
    return readPosition;
}

//------------------------------------------------------------------------------
void
winFileAccess::ListDirectory(const char* directory, Array<String>& entries) const {
    StringBuilder sb;
    sb.Format(256, "%s/*", directory);
    const char* wildcard = sb.AsCStr();

    struct _finddata_t findinfo;
    const intptr_t findhandle = _findfirst(wildcard, &findinfo);
    if (findhandle == -1)
        return;

    do {
        StringBuilder sbFull;
        sbFull.Format(256, "%s/%s", directory, findinfo.name);
        const char* search = sbFull.AsCStr();
        if (findinfo.attrib & _A_SUBDIR) {
            if (::strcmp(findinfo.name, ".") && ::strcmp(findinfo.name, "..")) {
                ListDirectory(search, entries);
            }
        } else {
            entries.Add(sbFull.GetString());
        }
    } while (_findnext(findhandle, &findinfo) != -1);

    _findclose(findhandle);
}

} // namespace _priv
} // namespace Oryol
