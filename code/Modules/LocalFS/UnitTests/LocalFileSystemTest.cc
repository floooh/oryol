//------------------------------------------------------------------------------
//  LocalFileSystemTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Core.h"
#include "Core/String/StringBuilder.h"
#include "IO/IO.h"
#include "LocalFS/LocalFileSystem.h"
#include "LocalFS/private/fsWrapper.h"
#include <thread>

using namespace Oryol;

static void
wait(const Ptr<IORequest>& msg) {
    while (!msg->Handled) {
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
    auto write = IOWrite::Create();
    write->Url = "root:test.txt";
    write->Data.Add((const uint8_t*)hello.AsCStr(), hello.Length());
    IO::Put(write);
    wait(write);
    CHECK(write->Status == IOStatus::OK);

    // read the file back
    auto read = IORead::Create();
    read->Url = "root:test.txt";
    IO::Put(read);
    wait(read);
    CHECK(read->Status == IOStatus::OK);
    CHECK(!read->Data.Empty());
    CHECK(read->Data.Size() == 12);
    String readStr((const char*)read->Data.Data(), 0, read->Data.Size());
    CHECK(readStr == "Hello World!");

    // read from offset to end of file
    read = IORead::Create();
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
    read = IORead::Create();
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

TEST(SameExtensionTest) {
    Core::Setup();
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("file", LocalFileSystem::Creator());
    IO::Setup(ioSetup);

    // write 2 files with the same extension
    const String helloJSON("Hello JSON! Bla bla bla bla");
    const String helloDDS("Hello DDS! Dupdidum");
    auto writeJSON = IOWrite::Create();
    writeJSON->Url = "root:hello.json";
    writeJSON->Data.Add((const uint8_t*)helloJSON.AsCStr(), helloJSON.Length());
    IO::Put(writeJSON);
    wait(writeJSON);
    auto writeDDS = IOWrite::Create();
    writeDDS->Url = "root:hello.dds";
    writeDDS->Data.Add((const uint8_t*)helloDDS.AsCStr(), helloDDS.Length());
    IO::Put(writeDDS);
    wait(writeDDS);

    // read the 2 files back, using the
    bool jsonDone = false, jsonSuccess = false;
    bool ddsDone = false, ddsSuccess = false;
    IO::Load("root:hello.dds",
        // success?
        [&ddsDone, &ddsSuccess](IO::LoadResult res) {
            String payload((const char*)res.Data.Data(), 0, res.Data.Size());
            Log::Info("1: Successfully loaded '%s': '%s'!\n", res.Url.Path().AsCStr(), payload.AsCStr());
            ddsDone = ddsSuccess = true;
        },
        // failure
        [&ddsDone](const URL& url, IOStatus::Code ioStatus) {
            Log::Info("1: Failed to load '%s' with '%s!\n", url.Path().AsCStr(), IOStatus::ToString(ioStatus));
            ddsDone = true;
        });
    IO::Load("root:hello.json",
        // success?
        [&jsonDone, &jsonSuccess](IO::LoadResult res) {
            String payload((const char*)res.Data.Data(), 0, res.Data.Size());
            Log::Info("1: Successfully loaded '%s': '%s'!\n", res.Url.Path().AsCStr(), payload.AsCStr());
            jsonDone = jsonSuccess = true;
        },
        // failure?
        [&jsonDone](const URL& url, IOStatus::Code ioStatus) {
            Log::Info("1: Failed to load '%s' with '%s!\n", url.Path().AsCStr(), IOStatus::ToString(ioStatus));
            jsonDone = true;
        });

    // pump the runloop until both files are loaded (or have failed)
    while (!(ddsDone && jsonDone)) {
        Core::PreRunLoop()->Run();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Core::PostRunLoop()->Run();
    }
    CHECK(ddsSuccess);
    CHECK(jsonSuccess);

    // and the same sequentially
    ddsDone = false; ddsSuccess = false;
    IO::Load("root:hello.dds",
        // success?
        [&ddsDone, &ddsSuccess](IO::LoadResult res) {
            String payload((const char*)res.Data.Data(), 0, res.Data.Size());
            Log::Info("2: Successfully loaded '%s': '%s'!\n", res.Url.Path().AsCStr(), payload.AsCStr());
            ddsDone = ddsSuccess = true;
        },
        // failure
        [&ddsDone](const URL& url, IOStatus::Code ioStatus) {
            Log::Info("2: Failed to load '%s' with '%s!\n", url.Path().AsCStr(), IOStatus::ToString(ioStatus));
            ddsDone = true;
        });
    while (!ddsDone) {
        Core::PreRunLoop()->Run();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Core::PostRunLoop()->Run();
    }
    CHECK(ddsSuccess);

    jsonDone = false; jsonSuccess = false;
    IO::Load("root:hello.json",
        // success?
        [&jsonDone, &jsonSuccess](IO::LoadResult res) {
            String payload((const char*)res.Data.Data(), 0, res.Data.Size());
            Log::Info("2: Successfully loaded '%s': '%s'!\n", res.Url.Path().AsCStr(), payload.AsCStr());
            jsonDone = jsonSuccess = true;
        },
        // failure?
        [&jsonDone](const URL& url, IOStatus::Code ioStatus) {
            Log::Info("2: Failed to load '%s' with '%s!\n", url.Path().AsCStr(), IOStatus::ToString(ioStatus));
            jsonDone = true;
        });
    // pump the runloop until both files are loaded (or have failed)
    while (!jsonDone) {
        Core::PreRunLoop()->Run();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Core::PostRunLoop()->Run();
    }
    CHECK(jsonSuccess);

    IO::Discard();
    Core::Discard();
}



