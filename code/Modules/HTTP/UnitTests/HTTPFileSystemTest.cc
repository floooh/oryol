//------------------------------------------------------------------------------
//  HTTPFileSystemTest.cc
//  Test HTTP file system functionality.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "HTTP/HTTPFileSystem.h"
#include "IO/IO.h"

using namespace Oryol;

#if !ORYOL_EMSCRIPTEN
TEST(HTTPFileSystemTest) {
    
    // setup an IO facade, and associate http: with the HTTPFileSystem
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    IO::Setup(ioSetup);
    
    // asynchronously load the index.html file
    Ptr<IOProtocol::Get> req = IO::LoadFile("http://www.flohofwoe.net/index.html");
    
    // trigger the runloop until the request has been handled
    while (!req->Handled()) {
        Core::PreRunLoop()->Run();
    }
    
    // check what we got
    CHECK(req->GetStatus() == IOStatus::OK);
    const Ptr<Stream>& loadedData = req->GetStream();
    CHECK(loadedData.isValid());
    CHECK(loadedData->Size() > 0);
    CHECK(loadedData->GetContentType().TypeAndSubType() == "text/html");
    loadedData->Open(OpenMode::ReadOnly);
    String content((const char*)loadedData->MapRead(nullptr), 0, loadedData->Size());
    Log::Info("%s\n", content.AsCStr());
    loadedData->Close();
    req = 0;
    
    IO::Discard();
}
#endif
