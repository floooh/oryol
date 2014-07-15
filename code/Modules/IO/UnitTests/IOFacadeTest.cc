//------------------------------------------------------------------------------
//  IOFacadeTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/IOFacade.h"
#include "Core/CoreFacade.h"
#include "IO/Stream/BinaryStreamReader.h"
#include "IO/Stream/BinaryStreamWriter.h"
#include "IO/Stream/MemoryStream.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;

std::atomic<int32> numGetHandled{0};
std::atomic<int32> numGetRangeHandled{0};

class TestFileSystem : public FileSystem {
    OryolClassDecl(TestFileSystem);
public:
    /// called when the IOProtocol::Get message is received
    virtual void onGet(const Core::Ptr<IOProtocol::Get>& msg) {
        Log::Info("TestFileSystem::onGet() called!\n");
        numGetHandled++;
        
        // create a stream object, and just write the URL from the msg to it
        Ptr<MemoryStream> stream = MemoryStream::Create();
        stream->Open(OpenMode::WriteOnly);
        Ptr<BinaryStreamWriter> writer = BinaryStreamWriter::Create(stream);
        writer->Write(msg->GetURL().Get());
        stream->Close();
        
        msg->SetStream(stream);
        msg->SetStatus(IOStatus::OK);
        msg->SetHandled();
    };
    /// called when the IOProtocol::GetRange message is received
    virtual void onGetRange(const Core::Ptr<IOProtocol::GetRange>& msg) {
        Log::Info("TestFileSystem::onGetRange() called!\n");
        numGetRangeHandled++;
        msg->SetHandled();
    }
};
OryolClassImpl(TestFileSystem);

TEST(IOFacadeTest) {
    IOFacade* ioFacade = IOFacade::CreateSingle();
    
    // register our test file-system as URI scheme "test"
    ioFacade->RegisterFileSystem("test", Creator<TestFileSystem,FileSystem>());
    
    // setup an assign which resolves to the test file system
    ioFacade->SetAssign("bla:", "test://blub.com/");
    
    // an URL which should resolve to test://blub.com/blob.txt
    URL url("bla:blob.txt");
    CHECK(url.IsValid());
    
    // asynchronously 'load' a file
    Ptr<IOProtocol::Get> msg = ioFacade->LoadFile(url);
    
    // trigger the runloop until our message is handled
    while (!msg->Handled()) {
        CoreFacade::Instance()->RunLoop()->Run();
    }
    CHECK(numGetHandled == 1);
    CHECK(numGetRangeHandled == 0);
    
    // check the msg result
    CHECK(msg->GetStream().isValid());
    CHECK(msg->GetStatus() == IOStatus::OK);
    const Ptr<Stream>& stream = msg->GetStream();
    stream->Open(OpenMode::ReadOnly);
    Ptr<BinaryStreamReader> reader = BinaryStreamReader::Create(stream);
    StringAtom str;
    CHECK(reader->Read(str));
    stream->Close();
    CHECK(str == msg->GetURL().Get());
    
    // FIXME: dynamically add/remove/replace filesystems, ...
    
    IOFacade::DestroySingle();
}
