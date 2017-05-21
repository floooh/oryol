//------------------------------------------------------------------------------
//  LocalFileSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "LocalFileSystem.h"
#include "Core/String/StringBuilder.h"
#include "LocalFS/private/fsWrapper.h"
#include "IO/IO.h"

namespace Oryol {

using namespace _priv;

//------------------------------------------------------------------------------
void
LocalFileSystem::init(const StringAtom& scheme_) {
    FileSystemBase::init(scheme_);
    StringBuilder strBuilder;

    // setup the root assign
    strBuilder.Format(4096, "%s:///%s", this->scheme.AsCStr(), fsWrapper::getExecutableDir().AsCStr());
    IO::SetAssign("root:", strBuilder.GetString());

    // setup the cwd assign
    strBuilder.Format(4096, "%s:///%s", this->scheme.AsCStr(), fsWrapper::getCwd().AsCStr());
    IO::SetAssign("cwd:", strBuilder.GetString());
}

//------------------------------------------------------------------------------
void
LocalFileSystem::onMsg(const Ptr<IORequest>& req) {
    if (req->IsA<IORead>()) {
        this->onRead(req->DynamicCast<IORead>());
    }
    else if (req->IsA<IOWrite>()) {
        this->onWrite(req->DynamicCast<IOWrite>());
    }
    req->Handled = true;
}

//------------------------------------------------------------------------------
void
LocalFileSystem::onRead(const Ptr<IORead>& msg) {
    if (msg->Url.HasPath()) {
        fsWrapper::handle h = fsWrapper::openRead(msg->Url.Path().AsCStr());
        if (fsWrapper::invalidHandle != h) {
            const int startOffset = msg->StartOffset;
            const int endOffset = msg->EndOffset;
            if (startOffset > 0) {
                fsWrapper::seek(h, startOffset);
            }
            int size;
            if (endOffset == EndOfFile) {
                size = fsWrapper::size(h) - startOffset;
            }
            else {
                size = endOffset - startOffset;
            }
            if (size > 0) {
                uint8_t* ptr = msg->Data.Add(size);
                int bytesRead = fsWrapper::read(h, ptr, size);
                if (bytesRead != size) {
                    msg->Status = IOStatus::DownloadError;
                    msg->ErrorDesc = "Fewer bytes read then expected";
                }
                else {
                    msg->Status = IOStatus::OK;
                }
            }
            fsWrapper::close(h);
        }
        else {
            msg->Status = IOStatus::NotFound;
            msg->ErrorDesc = "Failed to open file";
        }
    }
    else {
        msg->Status = IOStatus::BadRequest;
        msg->ErrorDesc = "No path in URL";
    }
}

//------------------------------------------------------------------------------
void
LocalFileSystem::onWrite(const Ptr<IOWrite>& msg) {
    if (msg->Url.HasPath()) {
        fsWrapper::handle h = fsWrapper::openWrite(msg->Url.Path().AsCStr());
        if (fsWrapper::invalidHandle != h) {
            if (!msg->Data.Empty()) {
                fsWrapper::write(h, msg->Data.Data(), msg->Data.Size());
            }
            fsWrapper::close(h);
            msg->Status = IOStatus::OK;
        }
        else {
            msg->Status = IOStatus::NotFound;
            msg->ErrorDesc = "Failed to open file";
        }
    }
    else {
        msg->Status = IOStatus::BadRequest;
        msg->ErrorDesc = "No path in URL";
    }
}

} // namespace Oryol

