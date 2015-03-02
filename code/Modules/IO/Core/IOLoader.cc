//------------------------------------------------------------------------------
//  IOLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IOLoader.h"

namespace Oryol {

OryolClassImpl(IOLoader);

//------------------------------------------------------------------------------
void
IOLoader::StartLoad(const Ptr<Stream>& /*dstStream*/, int32 /*expectedLoadSize*/) {
    // implement in subclass!
}

//------------------------------------------------------------------------------
void
IOLoader::Load(const void* /*data*/, int32 /*numBytes*/) {
    // implement in subclass!
}

//------------------------------------------------------------------------------
void
IOLoader::FinishLoad() {
    // implement in subclass!
}

//------------------------------------------------------------------------------
void
IOLoader::LoadFailed(const URL& url, int32 ioLane, IOStatus::Code ioStatus) {
    // implement in subclass!
}

} // namespace Oryol