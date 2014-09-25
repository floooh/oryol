//------------------------------------------------------------------------------
//  FileSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "FileSystem.h"
#include "Core/Log.h"

namespace Oryol {

OryolClassImpl(FileSystem);

//------------------------------------------------------------------------------
FileSystem::FileSystem() {
    // empty
}

//------------------------------------------------------------------------------
FileSystem::~FileSystem() {
    // empty
}

//------------------------------------------------------------------------------
void
FileSystem::onRequest(const Ptr<IOProtocol::Request>& msg) {
    // implement in subclass!
    Log::Warn("FileSystem::onRequest(): message not handled by FileSystem!\n");
}

//------------------------------------------------------------------------------
void
FileSystem::DoWork() {
    // implement in subclass!
}

} // namespace Oryol