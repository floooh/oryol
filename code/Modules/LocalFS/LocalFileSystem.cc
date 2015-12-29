//------------------------------------------------------------------------------
//  LocalFileSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "LocalFileSystem.h"

namespace Oryol {

//------------------------------------------------------------------------------
void
LocalFileSystem::Init() {
    Log::Info("FIXME: LocalFileSystem::Init()\n");
}

//------------------------------------------------------------------------------
void
LocalFileSystem::onRead(const Ptr<IOProtocol::Read>& msg) {
    Log::Info("FIXME: LocalFileSystem::onRead()\n");
}

//------------------------------------------------------------------------------
void
LocalFileSystem::onWrite(const Ptr<IOProtocol::Write>& msg) {
    Log::Info("FIXME: LocalFileSystem::onWrite()\n");
}

} // namespace Oryol

