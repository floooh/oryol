//------------------------------------------------------------------------------
//  IOFacadeTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/IO.h"
#include "IO/FileSystemBase.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "Core/Creator.h"

using namespace Oryol;

std::atomic<int> numRequestsHandled{0};

class TestFileSystem : public FileSystemBase {
    OryolClassDecl(TestFileSystem);
    OryolClassCreator(TestFileSystem);
public:
    /// called when the IOProtocol::Read message is received
    virtual void onMsg(const Ptr<IORequest>& msg) override {
        if (msg->IsA<IORead>()) {
            Log::Info("TestFileSystem::onMsg(IORequest) called!\n");
            numRequestsHandled++;
        
            Ptr<IORead> ioRead = msg->DynamicCast<IORead>();
            static const uint8_t payload[] = {'A', 'B', 'C', 'D'};
            ioRead->Data.Add(payload, sizeof(payload));
            ioRead->Status = IOStatus::OK;
        }
        msg->Handled = true;
    };
};

// try to find out which of the IO tests hangs in travis-ci
#if !ORYOL_EMSCRIPTEN && !ORYOL_UNITTESTS_HEADLESS
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
    Ptr<IORead> msg = IO::LoadFile(url);
    
    // trigger the runloop until our message is handled
    while (!msg->Handled) {
        Core::PreRunLoop()->Run();
    }
    CHECK(numRequestsHandled == 1);
    
    // check the msg result
    CHECK(!msg->Data.Empty());
    CHECK(msg->Data.Size() == 4);
    CHECK(msg->Status == IOStatus::OK);
    const uint8_t* payload = msg->Data.Data();
    CHECK(payload[0] == 'A');
    CHECK(payload[1] == 'B');
    CHECK(payload[2] == 'C');
    CHECK(payload[3] == 'D');

    // FIXME: dynamically add/remove/replace filesystems, ...
    
    IO::Discard();
    Core::Discard();
}
#endif
