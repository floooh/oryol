#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::baseFileAccess
    @ingroup _priv
    @brief a file access util class

    Implements helper methods around the HostFileHandle pointer.
*/
#include "Core/Containers/Array.h"
#include "Core/String/String.h"
#include "IO/Core/OpenMode.h"
#include "IO/Core/URL.h"
#include "IO/Stream/MemoryStream.h"
#include "Core/Ptr.h"
#include "Core/Types.h"

namespace Oryol {
namespace _priv {

enum class SeekType {
    Set,    ///< Seek from beginning of file
    End     ///< Seek from end of file.
};

// FIXME:
struct HostFileHandle {
	char dummy;
};

class baseFileAccess {
public:
    virtual ~baseFileAccess() {}

    virtual HostFileHandle* Open(const char *path, OpenMode::Enum mode) const;

    /// the HostFileHandle pointer is invalid after this was called
    virtual void Close(HostFileHandle* file) const;

    /// write a number of bytes to the file (returns bytes written)
    virtual int64 Write(HostFileHandle* file, const void* ptr, int64 bufferSize) const;

    /// read a number of bytes from the file (returns bytes read), bufferSize is the size
    /// of the allocated buffer behind the given pointer
    virtual int64 Read(HostFileHandle* file, void* ptr, int64 bufferSize) const;

    virtual int64 Tell(HostFileHandle* file) const;
    virtual int64 Seek(HostFileHandle* file, int64 pos, SeekType type) const;
    virtual int64 Length(HostFileHandle* file) const;

    virtual void ListDirectory(const char* directory, Array<String>& entries) const {}
protected:
    const char* ResolveMode(OpenMode::Enum mode) const;
};

} // namespace _priv
} // namespace Oryol
