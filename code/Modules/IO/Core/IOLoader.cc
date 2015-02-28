//------------------------------------------------------------------------------
//  IOLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IOLoader.h"

namespace Oryol {

OryolClassImpl(IOLoader);

//------------------------------------------------------------------------------
void
IOLoader::Loaded(const URL& url, int32 ioLane, const void* data, int32 numBytes) {
    // implement in subclass!
}

//------------------------------------------------------------------------------
void
IOLoader::Failed(const URL& url, int32 ioLane, IOStatus::Code ioStatus) {
    // implement in subclass!
}

} // namespace Oryol