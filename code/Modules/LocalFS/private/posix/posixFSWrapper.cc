//------------------------------------------------------------------------------
//  posixFSWrapper.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "posixFSWrapper.h"
#include "Core/String/StringBuilder.h"
#include <stdio.h>
#include "LocalFS/private/whereami/whereami.h"
#if ORYOL_WINDOWS
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace Oryol {
namespace _priv {

const posixFSWrapper::handle posixFSWrapper::invalidHandle = nullptr;

//------------------------------------------------------------------------------
posixFSWrapper::handle
posixFSWrapper::openRead(const char* path) {
    o_assert_dbg(path);
    return fopen(path, "rb");
}

//------------------------------------------------------------------------------
posixFSWrapper::handle
posixFSWrapper::openWrite(const char* path) {
    o_assert_dbg(path);
    return fopen(path, "wb");
}

//------------------------------------------------------------------------------
int
posixFSWrapper::write(handle h, const void* ptr, int numBytes) {
    o_assert_dbg(invalidHandle != h);
    o_assert_dbg(ptr);
    return (int) fwrite(ptr, 1, numBytes, (FILE*)h);
}

//------------------------------------------------------------------------------
int
posixFSWrapper::read(handle h, void* ptr, int numBytes) {
    o_assert_dbg(invalidHandle != h);
    o_assert_dbg(ptr);
    return (int) fread(ptr, 1, numBytes, (FILE*)h);
}

//------------------------------------------------------------------------------
bool
posixFSWrapper::seek(handle h, int offset) {
    o_assert_dbg(invalidHandle != h);
    return 0 == fseek((FILE*)h, offset, SEEK_SET);
}

//------------------------------------------------------------------------------
int
posixFSWrapper::size(handle h) {
    o_assert_dbg(invalidHandle != h);
    FILE* fp = (FILE*) h;
    long off = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, off, SEEK_SET);
    return (int) size;
}

//------------------------------------------------------------------------------
void
posixFSWrapper::close(handle h) {
    o_assert_dbg(invalidHandle != h);
    fclose((FILE*)h);
}

//------------------------------------------------------------------------------
String
posixFSWrapper::getExecutableDir() {
    char buf[4096];
    int length = wai_getExecutablePath(buf, sizeof(buf), nullptr);
    if (length > 0) {
        StringBuilder strBuilder(buf, 0, length);
        strBuilder.SubstituteAll("\\", "/");
        int slashIndex = strBuilder.FindLastOf(0, length, "/");
        o_assert((slashIndex > 0) && (slashIndex < (strBuilder.Length()-1)));
        String result(strBuilder.AsCStr(), 0, slashIndex+1);
        return result;
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
String
posixFSWrapper::getCwd() {
    char buf[4096];
    #if ORYOL_WINDOWS
    const char* cwd = _getcwd(buf, sizeof(buf));
    #else
    const char* cwd = getcwd(buf, sizeof(buf));
    #endif
    if (cwd) {
        StringBuilder strBuilder(cwd);
        strBuilder.Append('/');
        return strBuilder.GetString();
    }
    else {
        return String();
    }
}

} // namespace _priv
} // namespace Oryol

