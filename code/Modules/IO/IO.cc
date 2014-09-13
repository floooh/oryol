//------------------------------------------------------------------------------
//  IO.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IO.h"
#include "IO/Core/assignRegistry.h"
#include "Core/CoreFacade.h"
#include "Core/RunLoop.h"

namespace Oryol {

using namespace _priv;

IO::_state* IO::state = nullptr;

//------------------------------------------------------------------------------
void
IO::Setup(const IOSetup& setup) {
    o_assert(!IsValid());
    
    state = new _state();
    state->mainThreadId = std::this_thread::get_id();
    state->requestRouter = ioRequestRouter::Create(setup.NumIOLanes);
    
    // setup initial assigns
    for (const auto& assign : setup.Assigns) {
        SetAssign(assign.Key(), assign.Value());
    }
    
    // setup initial filesystems
    for (const auto& fs : setup.FileSystems) {
        RegisterFileSystem(fs.Key(), fs.Value());
    }
    
    state->runLoopId = CoreFacade::Instance()->RunLoop()->Add([] { doWork(); });
}

//------------------------------------------------------------------------------
void
IO::Discard() {
    o_assert(IsValid());
    CoreFacade::Instance()->RunLoop()->Remove(state->runLoopId);
    state->requestRouter = 0;
    delete state;
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
IO::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
void
IO::doWork() {
    o_assert_dbg(IsValid());
    o_assert_dbg(isMainThread());
    if (state->requestRouter.isValid()) {
        state->requestRouter->DoWork();
    }
}

//------------------------------------------------------------------------------
bool
IO::isMainThread() {
    o_assert_dbg(IsValid());
    return std::this_thread::get_id() == state->mainThreadId;
}

//------------------------------------------------------------------------------
void
IO::SetAssign(const String& assign, const String& path) {
    o_assert_dbg(IsValid());
    state->assignReg.SetAssign(assign, path);
}

//------------------------------------------------------------------------------
bool
IO::HasAssign(const String& assign) {
    o_assert_dbg(IsValid());
    return state->assignReg.HasAssign(assign);
}

//------------------------------------------------------------------------------
String
IO::LookupAssign(const String& assign) {
    o_assert_dbg(IsValid());
    return state->assignReg.LookupAssign(assign);
}

//------------------------------------------------------------------------------
String
IO::ResolveAssigns(const String& str) {
    return state->assignReg.ResolveAssigns(str);
}

//------------------------------------------------------------------------------
void
IO::RegisterFileSystem(const StringAtom& scheme, std::function<Ptr<FileSystem>()> fsCreator) {
    o_assert_dbg(IsValid());

    bool newFileSystem = !state->schemeReg.IsFileSystemRegistered(scheme);
    state->schemeReg.RegisterFileSystem(scheme, fsCreator);
    if (newFileSystem) {
        // notify IO threads that a filesystem was added
        Ptr<IOProtocol::notifyFileSystemAdded> msg = IOProtocol::notifyFileSystemAdded::Create();
        msg->SetScheme(scheme);
        state->requestRouter->Put(msg);
    }
    else {
        // notify IO threads that a filesystem was replaced
        Ptr<IOProtocol::notifyFileSystemReplaced> msg = IOProtocol::notifyFileSystemReplaced::Create();
        msg->SetScheme(scheme);
        state->requestRouter->Put(msg);
    }
}
    
//------------------------------------------------------------------------------
void
IO::UnregisterFileSystem(const StringAtom& scheme) {
    o_assert_dbg(IsValid());
    state->schemeReg.UnregisterFileSystem(scheme);
}

//------------------------------------------------------------------------------
bool
IO::IsFileSystemRegistered(const StringAtom& scheme) {
    o_assert_dbg(IsValid());
    return state->schemeReg.IsFileSystemRegistered(scheme);
}

//------------------------------------------------------------------------------
Ptr<IOProtocol::Get>
IO::LoadFile(const URL& url, int32 ioLane) {
    o_assert_dbg(IsValid());
    Ptr<IOProtocol::Get> ioReq = IOProtocol::Get::Create();
    ioReq->SetURL(url);
    ioReq->SetLane(ioLane);
    state->requestRouter->Put(ioReq);
    return ioReq;
}

//------------------------------------------------------------------------------
void
IO::Put(const Ptr<IOProtocol::Request>& ioReq) {
    o_assert_dbg(IsValid());
    state->requestRouter->Put(ioReq);
}

//------------------------------------------------------------------------------
schemeRegistry*
IO::getSchemeRegistry() {
    o_assert_dbg(IsValid());
    return &(state->schemeReg);
}

} // namespace Oryol
