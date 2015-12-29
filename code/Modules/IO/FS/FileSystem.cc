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
FileSystem::Init(const StringAtom& scheme_) {
    this->scheme = scheme_;
}

//------------------------------------------------------------------------------
void
FileSystem::InitLane() {
    // this is called per IO lane
}

//------------------------------------------------------------------------------
void
FileSystem::onRead(const Ptr<IOProtocol::Read>& msg) {
    // implement in subclass!
    o_warn("FileSystem::onRead(): message not handled by FileSystem!\n");
}

//------------------------------------------------------------------------------
void
FileSystem::onWrite(const Ptr<IOProtocol::Write>& msg) {
    // implement in subclass!
    o_warn("FileSystem::onWrite(): message not handled by FileSystem!\n");
}

} // namespace Oryol