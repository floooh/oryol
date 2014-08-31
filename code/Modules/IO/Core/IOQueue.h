#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::IOQueue
    @brief local async IO queue with success and fail callback
  
    TODO: documentation
    FIXME: should optionally add itself to global runloop, for
    scenarios where no on-frame function is available
*/
#include "Core/Types.h"
#include "IO/IOProtocol.h"
#include "Core/Containers/Array.h"
#include <functional>

namespace Oryol {
namespace IO {
    
class IOQueue {
public:
    /// callback function signature
    typedef std::function<void(const Core::Ptr<IO::Stream>&)> SuccessFunc;
    typedef std::function<void(const IO::URL& url, IOStatus::Code ioStatus)> FailFunc;

    /// global onFail callback
    FailFunc OnFail;
    /// add a file load request to the queue
    void Add(const URL& url, int32 ioLane, SuccessFunc onSuccess, FailFunc onFail=FailFunc());
    /// update the queue, call per frame
    void Update();
    /// return true if queue is empty
    bool Empty() const;
    
private:
    struct item {
        Core::Ptr<IO::IOProtocol::Get> ioRequest;
        SuccessFunc successFunc;
        FailFunc failFunc;
    };
    Core::Array<item> ioRequests;
};
    
} // namespace IO
} // namespace Oryol
