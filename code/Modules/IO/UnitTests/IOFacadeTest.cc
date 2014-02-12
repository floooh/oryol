//------------------------------------------------------------------------------
//  IOFacadeTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/IOFacade.h"
#include "Core/CoreFacade.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;

class TestFileSystem : public FileSystem {
    OryolClassDecl(TestFileSystem);
public:
    /// called when the IOProtocol::Get message is received
    virtual void onGet(const Core::Ptr<IOProtocol::Get>& msg) {
        Log::Info("TestFileSystem::onGet() called!\n");
        msg->SetHandled();
    };
    /// called when the IOProtocol::GetRange message is received
    virtual void onGetRange(const Core::Ptr<IOProtocol::GetRange>& msg) {
        Log::Info("TestFileSystem::onGetRange() called!\n");
        msg->SetHandled();
    }
};
OryolClassImpl(TestFileSystem);

TEST(IOFacadeTest) {
    IOFacade* ioFacade = IOFacade::CreateSingleton();
    
    // register our test file-system as URI scheme "test"
    ioFacade->RegisterFileSystem<TestFileSystem>("test", &TestFileSystem::Create<>);
    
    // setup an assign which resolves to the test file system
    ioFacade->SetAssign("bla:", "test://blub.com/");
    
    // an URL which should resolve to test://blub.com/blob.txt
    URL url("bla:blob.txt");
    CHECK(url.IsValid());
    
    // asynchronously 'load' a file
    Ptr<IOProtocol::Get> msg = ioFacade->LoadFile(url);
    
    // trigger the runloop until our message is handled
    while (!msg->Handled()) {
        CoreFacade::Instance()->ThreadRunLoop()->Run();
    }
    
    // FIXME: dynamically add/remove/replace filesystems, ...
    
    IOFacade::DestroySingleton();
}
