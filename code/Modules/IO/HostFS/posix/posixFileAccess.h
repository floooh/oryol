#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::posixFileAccess
    @ingroup _priv
    @brief private: Posix implementation of hostFileAccess
    @see hostFileAccess HostFileSystem
 */
#include "IO/HostFS/base/baseFileAccess.h"

namespace Oryol {
namespace _priv {

class posixFileAccess : public baseFileAccess {
public:
    HostFileHandle* Open(const char *path, OpenMode::Enum mode) const override;
    void Close(HostFileHandle* file) const override;
    int64 Write(HostFileHandle* file, const void* ptr, int64 bufferSize) const override;
    int64 Read(HostFileHandle* file, void* ptr, int64 bufferSize) const override;
    int64 Tell(HostFileHandle* file) const override;
    int64 Seek(HostFileHandle* file, int64 pos, SeekType type) const override;
    void ListDirectory(const char* directory, Array<String>& entries) const override;
};

} // namespace _priv
} // namespace Oryol
