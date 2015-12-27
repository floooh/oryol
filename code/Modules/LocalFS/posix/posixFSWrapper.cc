//------------------------------------------------------------------------------
//  posixFSWrapper.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "posixFSWrapper.h"
#include "LocalFS/whereami/whereami.h"
#include "Core/String/StringBuilder.h"
#include <stdio.h>
#include <unistd.h>

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
int32
posixFSWrapper::write(handle h, const void* ptr, int32 numBytes) {
    o_assert_dbg(invalidHandle != h);
    o_assert_dbg(ptr);
    return fwrite(ptr, 1, numBytes, (FILE*)h);
}

//------------------------------------------------------------------------------
int32
posixFSWrapper::read(handle h, void* ptr, int32 numBytes) {
    o_assert_dbg(invalidHandle != h);
    o_assert_dbg(ptr);
    return fread(ptr, 1, numBytes, (FILE*)h);
}

//------------------------------------------------------------------------------
bool
posixFSWrapper::seek(handle h, int32 offset) { 
    o_assert_dbg(invalidHandle != h);
    return 0 == fseek((FILE*)h, offset, SEEK_SET);
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
        int32 slashIndex = strBuilder.FindLastOf(0, length, "/");
        String result(strBuilder.AsCStr(), 0, slashIndex);
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
    const char* cwd = getcwd(buf, sizeof(buf));
    if (cwd) {
        return String(cwd);
    }
    else {
        return String();
    }
}

} // namespace _priv
} // namespace Oryol

