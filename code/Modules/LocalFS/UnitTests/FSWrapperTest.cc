//------------------------------------------------------------------------------
//  FSWrapperTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "LocalFS/private/fsWrapper.h"
#include "Core/String/StringBuilder.h"
#include <string.h>

using namespace Oryol;
using namespace _priv;

TEST(FSWrapperTest) {
    String exePath = fsWrapper::getExecutableDir();
    CHECK(!exePath.Empty());
    String cwdPath = fsWrapper::getCwd();
    CHECK(!cwdPath.Empty());

    StringBuilder strBuilder;
    strBuilder.Format(4096, "%s/test.txt", cwdPath.AsCStr());

    const fsWrapper::handle hw = fsWrapper::openWrite(strBuilder.AsCStr());
    CHECK(hw != fsWrapper::invalidHandle);
    const char* str = "Hello World\n";
    const int len = int(strlen(str));
    int bytesWritten = fsWrapper::write(hw, str, len);
    CHECK(bytesWritten == len);
    fsWrapper::close(hw);

    char buf[256];
    Memory::Clear(buf, sizeof(buf));
    const fsWrapper::handle hr = fsWrapper::openRead(strBuilder.AsCStr());
    CHECK(hr != fsWrapper::invalidHandle);
    int bytesRead = fsWrapper::read(hr, buf, sizeof(buf));
    CHECK(bytesRead == len);
    String readStr(buf, 0, len);
    CHECK(readStr == "Hello World\n");
    CHECK(fsWrapper::read(hr, buf, sizeof(buf)) == 0);
    CHECK(fsWrapper::seek(hr, 0));
    CHECK(fsWrapper::read(hr, buf, sizeof(buf)) == len);
    Memory::Clear(buf, sizeof(buf));
    CHECK(fsWrapper::seek(hr, 0));
    CHECK(fsWrapper::read(hr, buf, 5) == 5);
    readStr.Assign(buf, 0, 5);
    CHECK(readStr == "Hello");
    Memory::Clear(buf, sizeof(buf));
    CHECK(fsWrapper::seek(hr, 6));
    CHECK(fsWrapper::read(hr, buf, sizeof(buf)) == 6);
    readStr.Assign(buf, 0, 6);
    CHECK(readStr == "World\n");
    CHECK(fsWrapper::seek(hr, 128));
    CHECK(fsWrapper::read(hr, buf, sizeof(buf)) == 0);
    fsWrapper::close(hr);

    const fsWrapper::handle hs = fsWrapper::openRead(strBuilder.AsCStr());
    int size = fsWrapper::size(hs);
    CHECK(size == 12);
    CHECK(fsWrapper::seek(hs, 6));
    size = fsWrapper::size(hs);
    CHECK(size == 12);
    CHECK(fsWrapper::read(hs, buf, sizeof(buf)) == 6);
    readStr.Assign(buf, 0, 6);
    CHECK(readStr == "World\n");
    fsWrapper::close(hs);
}
