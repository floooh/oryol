#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IOQueue
    @ingroup IO
    @brief simplify asynchronous loading of multiple files

    IOQueues are used to load one or more files asynchronously, and associate
    a success (and optional failure) callback with an IO request.
    
    See the IOQueue sample application to see how it works :)
*/
#include "Core/Types.h"
#include "Core/String/StringAtom.h"
#include "IO/IOProtocol.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Buffer.h"
#include <functional>

namespace Oryol {
    
class IOQueue {
public:
    /// constructor
    IOQueue();
    /// destructor
    ~IOQueue();

    /// loading result
    struct Result {
        Result(const URL& url, Buffer&& data) : Url(url), Data(std::move(data)) {};

        URL Url;
        Buffer Data;
    };

    /// callback function signature for success
    typedef std::function<void(Result result)> SuccessFunc;
    /// callback function signature for success when loading URL groups
    typedef std::function<void(Array<Result>)> GroupSuccessFunc;
    /// callback function signature for failure
    typedef std::function<void(const URL& url, IOStatus::Code ioStatus)> FailFunc;

    /// start queue processing
    void Start();
    /// stop queue processing
    void Stop();
    /// return true if queue is in started state
    bool IsStarted() const;
    
    /// add a file load request to the queue
    void Add(const URL& url, SuccessFunc onSuccess, FailFunc onFail=FailFunc());
    /// add a file group request to the queue
    void AddGroup(const Array<URL>& urls, GroupSuccessFunc onSuccess, FailFunc onFail=FailFunc());
    /// return true if queue is empty
    bool Empty() const;
    
private:
    /// update the queue, called per frame from runloop
    void update();
    
    bool isStarted;
    int32 runLoopId;
    struct item {
        Ptr<IOProtocol::Request> ioRequest;
        SuccessFunc successFunc;
        FailFunc failFunc;
    };
    Array<item> items;
    struct groupItem {
        Array<Ptr<IOProtocol::Request>> ioRequests;
        GroupSuccessFunc successFunc;
        FailFunc failFunc;
    };
    Array<groupItem> groupItems;
};
    
} // namespace Oryol
