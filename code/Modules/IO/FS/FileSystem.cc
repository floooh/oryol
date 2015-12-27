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
    o_warn("FileSystem::onRequest(): message not handled by FileSystem!\n");
}

//------------------------------------------------------------------------------
void
FileSystem::Init() {
    // this is called once on the main thread
}

//------------------------------------------------------------------------------
void
FileSystem::InitLane() {
    // this is called per IO lane
}

//------------------------------------------------------------------------------
void
FileSystem::DoWork() {
    // implement in subclass!
}

} // namespace Oryol