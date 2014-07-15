//------------------------------------------------------------------------------
//  ioLane.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ioLane.h"
#include "Messaging/Dispatcher.h"
#include "IO/Core/schemeRegistry.h"

namespace Oryol {
namespace IO {

OryolClassImpl(ioLane);

using namespace Core;
using namespace Messaging;

//------------------------------------------------------------------------------
ioLane::ioLane() {
    // let our thread wake up from time to time
    this->SetTickDuration(100);
}

//------------------------------------------------------------------------------
ioLane::~ioLane() {
    o_assert(this->threadStopped);
}

//------------------------------------------------------------------------------
void
ioLane::onThreadEnter() {
    ThreadedQueue::onThreadEnter();

    // setup a Dispatcher to route messages to safely route messages
    // to this object's callback methods
    Ptr<Dispatcher<IOProtocol>> disp = Dispatcher<IOProtocol>::Create();
    using namespace std::placeholders;
    disp->Subscribe<IOProtocol::Get>(std::bind(&ioLane::onGet, this, _1));
    disp->Subscribe<IOProtocol::GetRange>(std::bind(&ioLane::onGetRange, this, _1));
    disp->Subscribe<IOProtocol::notifyFileSystemAdded>(std::bind(&ioLane::onNotifyFileSystemAdded, this, _1));
    disp->Subscribe<IOProtocol::notifyFileSystemReplaced>(std::bind(&ioLane::onNotifyFileSystemReplaced, this, _1));
    disp->Subscribe<IOProtocol::notifyFileSystemRemoved>(std::bind(&ioLane::onNotifyFileSystemRemoved, this, _1));
    this->forwardingPort = disp;
}

//------------------------------------------------------------------------------
void
ioLane::onThreadLeave() {
    this->forwardingPort = 0;
    this->fileSystems.Clear();
    ThreadedQueue::onThreadLeave();
}

//------------------------------------------------------------------------------
void
ioLane::onTick() {
    ThreadedQueue::onTick();
    
    // also tick our file systems
    for (const auto& kvp : this->fileSystems) {
        kvp.Value()->DoWork();
    }
}

//------------------------------------------------------------------------------
Ptr<FileSystem>
ioLane::fileSystemForURL(const URL& url) {
    StringAtom scheme = url.Scheme();
    if (this->fileSystems.Contains(scheme)) {
        return this->fileSystems[scheme];
    }
    else {
        Log::Warn("ioLane::fileSystemForURL: no filesystem registered for URL scheme '%s'!\n", scheme.AsCStr());
        return Ptr<FileSystem>();
    }
}

//------------------------------------------------------------------------------
void
ioLane::onGet(const Ptr<IOProtocol::Get>& msg) {
    if (msg->Cancelled()) {
        // message has been cancelled, don't waste time with it
        msg->SetStatus(IOStatus::Cancelled);
        msg->SetHandled();
    }
    else {
        Ptr<FileSystem> fs = this->fileSystemForURL(msg->GetURL());
        if (fs) {
            fs->onGet(msg);
        }
    }
}

//------------------------------------------------------------------------------
void
ioLane::onGetRange(const Ptr<IOProtocol::GetRange>& msg) {
    if (msg->Cancelled()) {
        // message has been cancelled, don't waste time with it
        msg->SetStatus(IOStatus::Cancelled);
        msg->SetHandled();
    }
    Ptr<FileSystem> fs = this->fileSystemForURL(msg->GetURL());
    if (fs) {
        fs->onGetRange(msg);
    }
}

//------------------------------------------------------------------------------
void
ioLane::onNotifyFileSystemAdded(const Ptr<IOProtocol::notifyFileSystemAdded>& msg) {
    const StringAtom& urlScheme = msg->GetScheme();
    o_assert(!this->fileSystems.Contains(urlScheme));
    Ptr<FileSystem> newFileSystem = schemeRegistry::Instance()->CreateFileSystem(urlScheme);
    this->fileSystems.Insert(urlScheme, newFileSystem);
}

//------------------------------------------------------------------------------
void
ioLane::onNotifyFileSystemReplaced(const Ptr<IOProtocol::notifyFileSystemReplaced>& msg) {
    const StringAtom& urlScheme = msg->GetScheme();
    o_assert(this->fileSystems.Contains(urlScheme));
    Ptr<FileSystem> newFileSystem = schemeRegistry::Instance()->CreateFileSystem(urlScheme);
    this->fileSystems[urlScheme] = newFileSystem;
}

//------------------------------------------------------------------------------
void
ioLane::onNotifyFileSystemRemoved(const Ptr<IOProtocol::notifyFileSystemRemoved>& msg) {
    const StringAtom& urlScheme = msg->GetScheme();
    o_assert(this->fileSystems.Contains(urlScheme));
    this->fileSystems.Erase(urlScheme);
}

} // namespace IO
} // namespace Oryol
