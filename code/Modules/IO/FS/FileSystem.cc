//------------------------------------------------------------------------------
//  FileSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "FileSystem.h"
#include "Core/Log.h"

namespace Oryol {
namespace IO {

OryolClassImpl(FileSystem);

using namespace Core;

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
FileSystem::onGet(const Ptr<IOProtocol::Get>& msg) {
    // implement in subclass!
    Log::Warn("FileSystem::onGet(): message not handled by FileSystem!\n");
}

//------------------------------------------------------------------------------
void
FileSystem::onGetRange(const Ptr<IOProtocol::GetRange>& msg) {
    // implement in subclass!
    Log::Warn("FileSystem::onGetRange(): message not handled by FileSystem!\n");
}

//------------------------------------------------------------------------------
void
FileSystem::DoWork() {
    // implement in subclass!
}

} // namespace IO
} // namespace Oryol