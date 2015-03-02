#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IOLoader
    @ingroup IO
    @brief base class for IO loaders
    
    IOLoader can be attached to an IO request object to process
    raw loaded data into a (Memory)Stream object on an IO thread. The
    default behaviour if no IOLoader is attached to an IO request
    is to simply copy the received data into a MemoryStream object which
    is returned in the IO request.
    
    IOLoaders can be used to move expensive file parsing off the main thread
    into one of the IO threads.
*/
#include "Core/RefCounted.h"
#include "IO/Stream/Stream.h"
#include "IO/Core/URL.h"
#include "IO/Core/IOStatus.h"

namespace Oryol {

class IOLoader : public RefCounted {
    OryolClassDecl(IOLoader);
public:
    // begin copying data, expectedSize can be 0 if size is not known yet
    virtual void StartLoad(const Ptr<Stream>& dstStream, int32 expectedLoadSize);
    /// load a chunk of data
    virtual void Load(const void* data, int32 numBytes);
    /// finish writing data, return stream object
    virtual void FinishLoad();
    /// called if loading had failed
    virtual void LoadFailed(const URL& url, int32 ioLane, IOStatus::Code ioStatus);
};

} // namespace Oryol