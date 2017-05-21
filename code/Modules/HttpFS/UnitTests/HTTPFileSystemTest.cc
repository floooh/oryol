//------------------------------------------------------------------------------
//  HTTPFileSystemTest.cc
//  Test HTTP file system functionality.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "HttpFS/HTTPFileSystem.h"
#include "IO/IO.h"

using namespace Oryol;

#if !ORYOL_EMSCRIPTEN && !ORYOL_UNITTESTS_HEADLESS
TEST(HTTPFileSystemTest) {
    Core::Setup();

    // setup an IO facade, and associate http: with the HTTPFileSystem
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    IO::Setup(ioSetup);
    
    // asynchronously load the index.html file
    Ptr<IORead> req = IO::LoadFile("http://www.flohofwoe.net/index.html");
    
    // trigger the runloop until the request has been handled
    while (!req->Handled) {
        Core::PreRunLoop()->Run();
    }
    
    // check what we got
    CHECK(req->Status == IOStatus::OK);
    CHECK(!req->Data.Empty());
    CHECK(req->Data.Size() > 0);
    if (req->Data.Size() > 0) {
        String content((const char*)req->Data.Data(), 0, req->Data.Size());
        Log::Info("%s\n", content.AsCStr());
    }
    req = 0;
    
    IO::Discard();
    Core::Discard();
}
#endif
