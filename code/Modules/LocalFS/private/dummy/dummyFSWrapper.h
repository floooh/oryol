#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::dummyFSWrapper
    @ingroup _priv
    @brief empty filesystem wrapper class
*/
#include "Core/Types.h"
#include "Core/String/String.h"

namespace Oryol {
namespace _priv {

class dummyFSWrapper  {
public:
    /// file-handle typedef
    typedef int handle;
    /// invalid file handle
    static const handle invalidHandle = 0;

    /// open file for reading
    static handle openRead(const char* path); 
    /// open file for writing
    static handle openWrite(const char* path);
    /// write to file, return number of bytes actually written
    static int write(handle f, const void* ptr, int numBytes);
    /// read from file, return number of bytes actually read
    static int read(handle f, void* ptr, int numBytes);
    /// seek from start of file
    static bool seek(handle f, int offset);
    /// get file size
    static int size(handle f);
    /// close file
    static void close(handle f);
    
    /// get path to own executable
    static String getExecutableDir();
    /// get current directory
    static String getCwd();
};

} // namespace _priv
} // namespace Oryol
