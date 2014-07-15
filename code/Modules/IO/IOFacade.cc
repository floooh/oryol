//------------------------------------------------------------------------------
//  IOFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IOFacade.h"
#include "IO/Core/assignRegistry.h"
#include "Core/CoreFacade.h"

namespace Oryol {
namespace IO {

using namespace Core;
    
OryolGlobalSingletonImpl(IOFacade);

/// @todo: make numIOLanes configurable
const int32 IOFacade::numIOLanes = 4;

//------------------------------------------------------------------------------
IOFacade::IOFacade() {
    this->SingletonEnsureUnique();
    this->mainThreadId = std::this_thread::get_id();
    assignRegistry::CreateSingle();
    schemeRegistry::CreateSingle();
    this->requestRouter = ioRequestRouter::Create(IOFacade::numIOLanes);
    CoreFacade::Instance()->RunLoop()->Add(RunLoop::Callback("IO::IOFacade", 0, std::bind(&IOFacade::doWork, this)));
}

//------------------------------------------------------------------------------
IOFacade::~IOFacade() {
    o_assert(this->isMainThread());
    CoreFacade::Instance()->RunLoop()->Remove("IO::IOFacade");
    this->requestRouter = 0;
    schemeRegistry::DestroySingle();
    assignRegistry::DestroySingle();
}

//------------------------------------------------------------------------------
void
IOFacade::doWork() {
    o_assert(this->isMainThread());
    if (this->requestRouter.isValid()) {
        this->requestRouter->DoWork();
    }
}

//------------------------------------------------------------------------------
bool
IOFacade::isMainThread() {
    return std::this_thread::get_id() == this->mainThreadId;
}

//------------------------------------------------------------------------------
void
IOFacade::SetAssign(const String& assign, const String& path) {
    assignRegistry::Instance()->SetAssign(assign, path);
}

//------------------------------------------------------------------------------
bool
IOFacade::HasAssign(const String& assign) const {
    return assignRegistry::Instance()->HasAssign(assign);
}

//------------------------------------------------------------------------------
String
IOFacade::LookupAssign(const String& assign) const {
    return assignRegistry::Instance()->LookupAssign(assign);
}

//------------------------------------------------------------------------------
String
IOFacade::ResolveAssigns(const String& str) const {
    return assignRegistry::Instance()->ResolveAssigns(str);
}

//------------------------------------------------------------------------------
void
IOFacade::RegisterFileSystem(const Core::StringAtom& scheme, CreatorRef<FileSystem> fsCreator) {
    schemeRegistry* reg = schemeRegistry::Instance();
    bool newFileSystem = !reg->IsFileSystemRegistered(scheme);
    reg->RegisterFileSystem(scheme, fsCreator);
    if (newFileSystem) {
        // notify IO threads that a filesystem was added
        Core::Ptr<IOProtocol::notifyFileSystemAdded> msg = IOProtocol::notifyFileSystemAdded::Create();
        msg->SetScheme(scheme);
        this->requestRouter->Put(msg);
    }
    else {
        // notify IO threads that a filesystem was replaced
        Core::Ptr<IOProtocol::notifyFileSystemReplaced> msg = IOProtocol::notifyFileSystemReplaced::Create();
        msg->SetScheme(scheme);
        this->requestRouter->Put(msg);
    }
}
    
//------------------------------------------------------------------------------
void
IOFacade::UnregisterFileSystem(const StringAtom& scheme) {
    schemeRegistry::Instance()->UnregisterFileSystem(scheme);
}

//------------------------------------------------------------------------------
bool
IOFacade::IsFileSystemRegistered(const StringAtom& scheme) const {
    return schemeRegistry::Instance()->IsFileSystemRegistered(scheme);
}

//------------------------------------------------------------------------------
Ptr<IOProtocol::Get>
IOFacade::LoadFile(const URL& url, int32 ioLane) {
    Ptr<IOProtocol::Get> ioReq = IOProtocol::Get::Create();
    ioReq->SetURL(url);
    ioReq->SetLane(ioLane);
    this->requestRouter->Put(ioReq);
    return ioReq;
}

//------------------------------------------------------------------------------
Ptr<IOProtocol::GetRange>
IOFacade::LoadFileRange(const URL& url, int32 startOffset, int32 endOffset, int32 ioLane) {
    Ptr<IOProtocol::GetRange> ioReq = IOProtocol::GetRange::Create();
    ioReq->SetURL(url);
    ioReq->SetLane(ioLane);
    ioReq->SetStartOffset(startOffset);
    ioReq->SetEndOffset(endOffset);
    this->requestRouter->Put(ioReq);
    return ioReq;
}

//------------------------------------------------------------------------------
void
IOFacade::AddPreloadFile(const URL& url, int32 ioLane) {
    /// @todo: implement AddPreloadFile()
}

//------------------------------------------------------------------------------
bool
IOFacade::IsPreloadingFinished() const {
    /// @todo: implement IsPreloadingFinished()
    return false;
}
    
} // namespace IO
} // namespace Oryol