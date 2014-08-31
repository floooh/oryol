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
#include "Core/String/StringAtom.h"
#include "IO/IOProtocol.h"
#include "Core/Containers/Array.h"
#include "Core/RunLoop.h"
#include <functional>

namespace Oryol {
namespace IO {
    
class IOQueue {
public:
    /// constructor
    IOQueue();
    /// destructor
    ~IOQueue();

    /// callback function signature for success
    typedef std::function<void(const Core::Ptr<IO::Stream>&)> SuccessFunc;
    /// callback function signature for failure
    typedef std::function<void(const IO::URL& url, IOStatus::Code ioStatus)> FailFunc;

    /// start queue processing
    void Start();
    /// stop queue processing
    void Stop();
    /// return true if queue is in started state
    bool IsStarted() const;
    
    /// add a file load request to the queue
    void Add(const URL& url, SuccessFunc onSuccess, FailFunc onFail=FailFunc());
    /// return true if queue is empty
    bool Empty() const;
    
private:
    /// update the queue, called per frame from runloop
    void update();
    
    bool isStarted;
    Core::RunLoop::Id runLoopId;
    struct item {
        Core::Ptr<IO::IOProtocol::Get> ioRequest;
        SuccessFunc successFunc;
        FailFunc failFunc;
    };
    Core::Array<item> ioRequests;
};
    
} // namespace IO
} // namespace Oryol
