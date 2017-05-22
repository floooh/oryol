//------------------------------------------------------------------------------
//  FileSystemBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "FileSystemBase.h"
#include "Core/Log.h"

namespace Oryol {

//------------------------------------------------------------------------------
FileSystemBase::~FileSystemBase() {
    // empty
}

//------------------------------------------------------------------------------
void
FileSystemBase::init(const StringAtom& scheme_) {
    this->scheme = scheme_;
}

//------------------------------------------------------------------------------
void
FileSystemBase::initLane() {
    // this is called per IO lane
}

//------------------------------------------------------------------------------
void
FileSystemBase::onMsg(const Ptr<IORequest>& ioReq) {
    o_warn("FileSystem::onMsg(): message not handled by FileSystem!\n");
}

} // namespace Oryol
