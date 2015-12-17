//------------------------------------------------------------------------------
//  posixFileAccess.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "posixFileAccess.h"
#include "Core/String/StringBuilder.h"
#include <limits>
#include <cstring>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace Oryol {
namespace _priv {

#define FORWARD(file) reinterpret_cast<HostFileHandle*>(file)
#define BACKWARD(handle) reinterpret_cast<FILE*>(handle)

//------------------------------------------------------------------------------
HostFileHandle*
posixFileAccess::Open(const char *path, OpenMode::Enum mode) const {
    const char *modeStr = ResolveMode(mode);
    if (modeStr == nullptr)
        return nullptr;
    return FORWARD(::fopen(path, modeStr));
}

//------------------------------------------------------------------------------
void
posixFileAccess::Close(HostFileHandle* file) const {
    ::fclose(BACKWARD(file));
}

//------------------------------------------------------------------------------
int64
posixFileAccess::Tell(HostFileHandle* file) const {
    const int64 pos = ::ftell(BACKWARD(file));
    return pos;
}

//------------------------------------------------------------------------------
int64
posixFileAccess::Seek(HostFileHandle* file, int64 pos, SeekType type) const {
    int mode = SEEK_SET;
    if (SeekType::End == type)
        mode = SEEK_END;
    return ::fseek(BACKWARD(file), pos, mode);
}

//------------------------------------------------------------------------------
int64
posixFileAccess::Write(HostFileHandle* file, const void* ptr, int64 numBytes) const {
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
posixFileAccess::Read(HostFileHandle* file, void* ptr, int64 bufferSize) const {
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

void
posixFileAccess::ListDirectory(const char* directory, Array<String>& entries) const {
    DIR *dir = ::opendir(directory);
    if (dir == nullptr)
        return;

    struct dirent *d;
    while ((d = ::readdir(dir)) != nullptr) {
        StringBuilder sb;
        sb.Format(256, "%s/%s", directory, d->d_name);
        const char* search = sb.AsCStr();
        struct stat st;
        if (::stat(search, &st) == -1)
            continue;
        if (st.st_mode & S_IFDIR) {
            if (::strcmp(d->d_name, ".") && ::strcmp(d->d_name, "..")) {
                ListDirectory(search, entries);
            }
        } else {
            entries.Add(sb.GetString());
        }
    }

    ::closedir(dir);
}

} // namespace _priv
} // namespace Oryol
