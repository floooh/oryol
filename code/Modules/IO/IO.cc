//------------------------------------------------------------------------------
//  IO.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IO.h"
#include "IO/Core/assignRegistry.h"
#include "IO/Core/ioPointers.h"
#include "Core/Core.h"
#include "IO/FS/ioRouter.h"
#include "IO/Core/assignRegistry.h"
#include "IO/Core/schemeRegistry.h"
#include "IO/Core/loadQueue.h"
#include "Core/RunLoop.h"

namespace Oryol {

using namespace _priv;

struct io_state {
    _priv::assignRegistry assignReg;
    _priv::schemeRegistry schemeReg;
    _priv::ioRouter router;
    RunLoop::Id runLoopId = RunLoop::InvalidId;
    class loadQueue loadQueue;
};
io_state* ioState = nullptr;

//------------------------------------------------------------------------------
void
IO::Setup(const IOSetup& setup) {
    o_assert(!IsValid());

    ioState = Memory::New<io_state>();
    ioPointers ptrs;
    ptrs.schemeRegistry = &ioState->schemeReg;
    ptrs.assignRegistry = &ioState->assignReg;
    ioState->router.setup(ptrs);

    // setup initial assigns
    for (const auto& assign : setup.Assigns) {
        SetAssign(assign.Key(), assign.Value());
    }
    
    // setup initial filesystems
    for (const auto& fs : setup.FileSystems) {
        RegisterFileSystem(fs.Key(), fs.Value());
    }

    ioState->runLoopId = Core::PreRunLoop()->Add([] { doWork(); });
}

//------------------------------------------------------------------------------
void
IO::Discard() {
    o_assert(IsValid());
    Core::PreRunLoop()->Remove(ioState->runLoopId);
    ioState->router.discard();
    Memory::Delete(ioState);
    ioState = nullptr;
}

//------------------------------------------------------------------------------
void
IO::doWork() {
    o_assert_dbg(IsValid());
    o_assert_dbg(Core::IsMainThread());
    ioState->router.doWork();
    ioState->loadQueue.update();
}

//------------------------------------------------------------------------------
bool
IO::IsValid() {
    return nullptr != ioState;
}

//------------------------------------------------------------------------------
void
IO::SetAssign(const String& assign, const String& path) {
    o_assert_dbg(IsValid());
    ioState->assignReg.SetAssign(assign, path);
}

//------------------------------------------------------------------------------
bool
IO::HasAssign(const String& assign) {
    o_assert_dbg(IsValid());
    return ioState->assignReg.HasAssign(assign);
}

//------------------------------------------------------------------------------
String
IO::LookupAssign(const String& assign) {
    o_assert_dbg(IsValid());
    return ioState->assignReg.LookupAssign(assign);
}

//------------------------------------------------------------------------------
String
IO::ResolveAssigns(const String& str) {
    return ioState->assignReg.ResolveAssigns(str);
}

//------------------------------------------------------------------------------
void
IO::RegisterFileSystem(const StringAtom& scheme, std::function<Ptr<FileSystem>()> fsCreator) {
    o_assert_dbg(IsValid());

    bool newFileSystem = !ioState->schemeReg.IsFileSystemRegistered(scheme);
    ioState->schemeReg.RegisterFileSystem(scheme, fsCreator);
    if (newFileSystem) {
        // notify IO threads that a filesystem was added
        Ptr<notifyFileSystemAdded> msg = notifyFileSystemAdded::Create();
        msg->Scheme = scheme;
        ioState->router.put(msg);
    }
    else {
        // notify IO threads that a filesystem was replaced
        Ptr<notifyFileSystemReplaced> msg = notifyFileSystemReplaced::Create();
        msg->Scheme = scheme;
        ioState->router.put(msg);
    }
}
    
//------------------------------------------------------------------------------
void
IO::UnregisterFileSystem(const StringAtom& scheme) {
    o_assert_dbg(IsValid());
    ioState->schemeReg.UnregisterFileSystem(scheme);
}

//------------------------------------------------------------------------------
bool
IO::IsFileSystemRegistered(const StringAtom& scheme) {
    o_assert_dbg(IsValid());
    return ioState->schemeReg.IsFileSystemRegistered(scheme);
}

//------------------------------------------------------------------------------
void
IO::Load(const URL& url, LoadSuccessFunc onSuccess, LoadFailedFunc onFailed) {
    o_assert_dbg(IsValid());
    ioState->loadQueue.add(url, onSuccess, onFailed);
}

//------------------------------------------------------------------------------
void
IO::LoadGroup(const Array<URL>& urls, LoadGroupSuccessFunc onSuccess, LoadFailedFunc onFailed) {
    o_assert_dbg(IsValid());
    ioState->loadQueue.addGroup(urls, onSuccess, onFailed);
}

//------------------------------------------------------------------------------
int
IO::NumPendingLoads() {
    o_assert_dbg(IsValid());
    return ioState->loadQueue.numPending();
}

//------------------------------------------------------------------------------
Ptr<IORead>
IO::LoadFile(const URL& url) {
    o_assert_dbg(IsValid());
    Ptr<IORead> ioReq = IORead::Create();
    ioReq->Url = url;
    ioState->router.put(ioReq);
    return ioReq;
}

//------------------------------------------------------------------------------
Ptr<IOWrite>
IO::WriteFile(const URL& url, const Buffer& data) {
    o_assert_dbg(IsValid());
    Ptr<IOWrite> ioReq = IOWrite::Create();
    ioReq->Url = url;
    ioReq->Data.Add(data.Data(), data.Size());
    ioState->router.put(ioReq);
    return ioReq;
}

//------------------------------------------------------------------------------
void
IO::Put(const Ptr<IORequest>& ioReq) {
    o_assert_dbg(IsValid());
    ioState->router.put(ioReq);
}

} // namespace Oryol
