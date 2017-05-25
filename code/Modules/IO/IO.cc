//------------------------------------------------------------------------------
//  IO.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IO.h"
#include "IO/private/assignRegistry.h"
#include "IO/private/ioPointers.h"
#include "Core/Core.h"
#include "IO/private/ioRouter.h"
#include "IO/private/assignRegistry.h"
#include "IO/private/schemeRegistry.h"
#include "IO/private/loadQueue.h"
#include "Core/RunLoop.h"

namespace Oryol {

using namespace _priv;

namespace {
    struct _state {
        _priv::assignRegistry assignReg;
        _priv::schemeRegistry schemeReg;
        _priv::ioRouter router;
        RunLoop::Id runLoopId = RunLoop::InvalidId;
        class loadQueue loadQueue;
    };
    _state* state = nullptr;
}

//------------------------------------------------------------------------------
void
IO::Setup(const IOSetup& setup) {
    o_assert(!IsValid());

    state = Memory::New<_state>();
    ioPointers ptrs;
    ptrs.schemeRegistry = &state->schemeReg;
    ptrs.assignRegistry = &state->assignReg;
    state->router.setup(ptrs);

    // setup initial assigns
    for (const auto& assign : setup.Assigns) {
        SetAssign(assign.Key(), assign.Value());
    }
    
    // setup initial filesystems
    for (const auto& fs : setup.FileSystems) {
        RegisterFileSystem(fs.Key(), fs.Value());
    }

    state->runLoopId = Core::PreRunLoop()->Add([] { doWork(); });
}

//------------------------------------------------------------------------------
void
IO::Discard() {
    o_assert(IsValid());
    Core::PreRunLoop()->Remove(state->runLoopId);
    state->router.discard();
    Memory::Delete(state);
    state = nullptr;
}

//------------------------------------------------------------------------------
void
IO::doWork() {
    o_assert_dbg(IsValid());
    o_assert_dbg(Core::IsMainThread());
    state->router.doWork();
    state->loadQueue.update();
}

//------------------------------------------------------------------------------
bool
IO::IsValid() {
    return nullptr != state;
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
IO::RegisterFileSystem(const StringAtom& scheme, std::function<Ptr<FileSystemBase>()> fsCreator) {
    o_assert_dbg(IsValid());

    bool newFileSystem = !state->schemeReg.IsFileSystemRegistered(scheme);
    state->schemeReg.RegisterFileSystem(scheme, fsCreator);
    if (newFileSystem) {
        // notify IO threads that a filesystem was added
        Ptr<notifyFileSystemAdded> msg = notifyFileSystemAdded::Create();
        msg->Scheme = scheme;
        state->router.put(msg);
    }
    else {
        // notify IO threads that a filesystem was replaced
        Ptr<notifyFileSystemReplaced> msg = notifyFileSystemReplaced::Create();
        msg->Scheme = scheme;
        state->router.put(msg);
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
void
IO::Load(const URL& url, LoadSuccessFunc onSuccess, LoadFailedFunc onFailed) {
    o_assert_dbg(IsValid());
    state->loadQueue.add(url, onSuccess, onFailed);
}

//------------------------------------------------------------------------------
void
IO::LoadGroup(const Array<URL>& urls, LoadGroupSuccessFunc onSuccess, LoadFailedFunc onFailed) {
    o_assert_dbg(IsValid());
    state->loadQueue.addGroup(urls, onSuccess, onFailed);
}

//------------------------------------------------------------------------------
int
IO::NumPendingLoads() {
    o_assert_dbg(IsValid());
    return state->loadQueue.numPending();
}

//------------------------------------------------------------------------------
Ptr<IORead>
IO::LoadFile(const URL& url) {
    o_assert_dbg(IsValid());
    Ptr<IORead> ioReq = IORead::Create();
    ioReq->Url = url;
    state->router.put(ioReq);
    return ioReq;
}

//------------------------------------------------------------------------------
Ptr<IOWrite>
IO::WriteFile(const URL& url, const Buffer& data) {
    o_assert_dbg(IsValid());
    Ptr<IOWrite> ioReq = IOWrite::Create();
    ioReq->Url = url;
    ioReq->Data.Add(data.Data(), data.Size());
    state->router.put(ioReq);
    return ioReq;
}

//------------------------------------------------------------------------------
void
IO::Put(const Ptr<IORequest>& ioReq) {
    o_assert_dbg(IsValid());
    state->router.put(ioReq);
}

} // namespace Oryol
