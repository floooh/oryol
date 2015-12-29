//------------------------------------------------------------------------------
//  LocalFileSystemTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Core.h"
#include "Core/String/StringBuilder.h"
#include "IO/IO.h"
#include "LocalFS/LocalFileSystem.h"
#include "LocalFS/Core/fsWrapper.h"

using namespace Oryol;

static void
wait(const Ptr<IOProtocol::Request>& msg) {
    while (!msg->Handled()) {
        Core::PreRunLoop()->Run();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Core::PostRunLoop()->Run();
    }
}

TEST(LocalFileSystemTest) {

    Core::Setup();
    IOSetup ioSetup;
    ioSetup.Assigns.Add("bla:", "root:bla/");
    ioSetup.Assigns.Add("blub:", "cwd:blub/");
    ioSetup.FileSystems.Add("file", LocalFileSystem::Creator());
    IO::Setup(ioSetup);

    // check whether assigns have been set
    StringBuilder strBuilder;
    strBuilder.Format(4096, "file:///%sbla/test.txt", _priv::fsWrapper::getExecutableDir().AsCStr());
    String bla = strBuilder.GetString();
    strBuilder.Format(4096, "file:///%sblub/test.txt", _priv::fsWrapper::getCwd().AsCStr());
    String blub = strBuilder.GetString();
    CHECK(bla == IO::ResolveAssigns("bla:test.txt"));
    CHECK(blub == IO::ResolveAssigns("blub:test.txt"));

    // write a file
    const String hello("Hello World!");
    auto write = IOProtocol::Write::Create();
    write->Url = "root:test.txt";
    write->Data.Add((const uint8*)hello.AsCStr(), hello.Length());
    IO::Put(write);
    wait(write);
    CHECK(write->Status == IOStatus::OK);

    // read the file back
    auto read = IOProtocol::Read::Create();
    read->Url = "root:test.txt";
    IO::Put(read);
    wait(read);
    CHECK(read->Status == IOStatus::OK);
    CHECK(!read->Data.Empty());
    CHECK(read->Data.Size() == 12);
    String readStr((const char*)read->Data.Data(), 0, read->Data.Size());
    CHECK(readStr == "Hello World!");

    // read from offset to end of file
    read = IOProtocol::Read::Create();
    read->Url = "root:test.txt";
    read->StartOffset = 6;
    IO::Put(read);
    wait(read);
    CHECK(read->Status == IOStatus::OK);
    CHECK(!read->Data.Empty());
    CHECK(read->Data.Size() == 6);
    readStr.Assign((const char*)read->Data.Data(), 0, read->Data.Size());
    CHECK(readStr == "World!");

    // read partial data
    read = IOProtocol::Read::Create();
    read->Url = "root:test.txt";
    read->StartOffset = 6;
    read->EndOffset = 11;
    IO::Put(read);
    wait(read);
    CHECK(read->Status == IOStatus::OK);
    CHECK(!read->Data.Empty());
    CHECK(read->Data.Size() == 5);
    readStr.Assign((const char*)read->Data.Data(), 0, read->Data.Size());
    CHECK(readStr == "World");

    IO::Discard();
    Core::Discard();
}


