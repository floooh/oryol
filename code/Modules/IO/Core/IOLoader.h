#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IOLoader
    @ingroup IO
    @brief base class for IO loaders
    
    An IOLoader object can be used to short-circuit asynchronous
    resource loading and potentially get rid of an extra memory copy
    (depending on the target platform). When a download is finished,
    the Loaded() method will be called with a direct pointer to the
    data in memory.
    
    NOTE: the Load() method will be called from a different thread
    then the main thread (depending on the target platform)!
*/
#include "Core/RefCounted.h"
#include "IO/Core/URL.h"
#include "IO/Core/IOStatus.h"

namespace Oryol {

class IOLoader : public RefCounted {
    OryolClassDecl(IOLoader);
public:
    /// data is ready, perform loading (can be called from other thread!)
    virtual void Loaded(const URL& url, int32 ioLane, const void* data, int32 numBytes);
    /// loading has failed
    virtual void Failed(const URL& url, IOStatus::Code ioStatus);
};

} // namespace Oryol