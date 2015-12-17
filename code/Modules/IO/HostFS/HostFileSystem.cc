//------------------------------------------------------------------------------
//  HostFileSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "HostFileSystem.h"
#include "Core/Log.h"
#include "IO/Core/IOStatus.h"

namespace Oryol {

OryolClassImpl(HostFileSystem);

//------------------------------------------------------------------------------
HostFileSystem::HostFileSystem() {
    // empty
}

//------------------------------------------------------------------------------
HostFileSystem::~HostFileSystem() {
    // empty
}

//------------------------------------------------------------------------------
void
HostFileSystem::onRequest(const Ptr<IOProtocol::Request>& msg) {
    if (msg->Cancelled()) {
        return;
    }
    this->requestQueue.Enqueue(msg);
}

//------------------------------------------------------------------------------
void
HostFileSystem::DoWork() {
    while (!this->requestQueue.Empty()) {
        const Ptr<IOProtocol::Request>& msg = this->requestQueue.Dequeue();
        const char* path = msg->GetURL().Path().AsCStr();
        _priv::HostFileHandle* file = hostFileAccess.Open(path, OpenMode::ReadOnly);
        if (file == nullptr) {
            msg->SetHandled();
            msg->SetStatus(IOStatus::NotFound);
            msg->SetErrorDesc("File not found or not readable");
            continue;
        }
        int64 length = hostFileAccess.Length(file);
        // partial file reading
        if (msg->GetEndOffset() > 0) {
            const int32 numBytes = msg->GetEndOffset() - msg->GetStartOffset();
            if (msg->GetStartOffset() + numBytes > length)
                length -= msg->GetStartOffset();
            else if (length < numBytes)
                length = numBytes;
        }
        hostFileAccess.Seek(file, msg->GetStartOffset(), _priv::SeekType::Set);
        // you should not try to load that much data into the memory - stream it on your own
        o_assert(length <= std::numeric_limits<int32>::max());
        Ptr<MemoryStream> stream = MemoryStream::Create(static_cast<int32>(length));
        hostFileAccess.Read(file, static_cast<void*>(stream->RawBuffer()), length);
        hostFileAccess.Close(file);
        msg->SetStream(stream);
        msg->SetStatus(IOStatus::OK);
        msg->SetHandled();
    }
}

} // namespace Oryol
