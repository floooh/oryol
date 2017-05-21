//------------------------------------------------------------------------------
//  FileSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "FileSystem.h"
#include "Core/Log.h"

namespace Oryol {

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
FileSystem::init(const StringAtom& scheme_) {
    this->scheme = scheme_;
}

//------------------------------------------------------------------------------
void
FileSystem::initLane() {
    // this is called per IO lane
}

//------------------------------------------------------------------------------
void
FileSystem::onMsg(const Ptr<IORequest>& ioReq) {
    o_warn("FileSystem::onMsg(): message not handled by FileSystem!\n");
}

} // namespace Oryol