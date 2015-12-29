//------------------------------------------------------------------------------
//  IOFacadeTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/IO.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"

using namespace Oryol;

std::atomic<int32> numRequestsHandled{0};

class TestFileSystem : public FileSystem {
    OryolClassDecl(TestFileSystem);
    OryolClassCreator(TestFileSystem);
public:
    /// called when the IOProtocol::Read message is received
    virtual void onRead(const Ptr<IOProtocol::Read>& msg) override {
        Log::Info("TestFileSystem::onRead() called!\n");
        numRequestsHandled++;
        
        // write payload data
        static const uint8 payload[] = {'A', 'B', 'C', 'D'};
        msg->Data.Add(payload, sizeof(payload));
        msg->Status = IOStatus::OK;
        msg->SetHandled();
    };
};
OryolClassImpl(TestFileSystem);

TEST(IOFacadeTest) {
    Core::Setup();
    IO::Setup(IOSetup());
    
    // register our test file-system as URI scheme "test"
    IO::RegisterFileSystem("test", TestFileSystem::Creator());
    
    // setup an assign which resolves to the test file system
    IO::SetAssign("bla:", "test://blub.com/");
    
    // an URL which should resolve to test://blub.com/blob.txt
    URL url("bla:blob.txt");
    CHECK(url.IsValid());
    
    // asynchronously 'load' a file
    Ptr<IOProtocol::Read> msg = IO::LoadFile(url);
    
    // trigger the runloop until our message is handled
    while (!msg->Handled()) {
        Core::PreRunLoop()->Run();
    }
    CHECK(numRequestsHandled == 1);
    
    // check the msg result
    CHECK(!msg->Data.Empty());
    CHECK(msg->Data.Size() == 4);
    CHECK(msg->Status == IOStatus::OK);
    const uint8* payload = msg->Data.Data();
    CHECK(payload[0] == 'A');
    CHECK(payload[1] == 'B');
    CHECK(payload[2] == 'C');
    CHECK(payload[3] == 'D');

    // FIXME: dynamically add/remove/replace filesystems, ...
    
    IO::Discard();
    Core::Discard();
}
