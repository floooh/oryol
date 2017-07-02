#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::loadQueue
    @ingroup IO
    @brief asynchronously load multiple files, invoke callbacks with result

    This is the class behind the IO::Load() and LoadGroup() functions.
*/
#include "Core/Types.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Buffer.h"
#include "IO/IOTypes.h"
#include "IO/private/ioRequests.h"
#include <functional>

namespace Oryol {
    
class loadQueue {
public:
    /// loading result (iff successful)
    struct result {
        result(const URL& url, Buffer&& data) : Url(url), Data(std::move(data)) { };
        result(result&& rhs) {
            this->Url = std::move(rhs.Url);
            this->Data = std::move(rhs.Data);
        };
        void operator=(result&& rhs) {
            this->Url = std::move(rhs.Url);
            this->Data = std::move(rhs.Data);            
        };
        URL Url;
        Buffer Data;
    };

    /// callback function signature for success
    typedef std::function<void(result result)> successFunc;
    /// callback function signature for success when loading URL groups
    typedef std::function<void(Array<result>)> groupSuccessFunc;
    /// callback function signature for failure
    typedef std::function<void(const URL& url, IOStatus::Code ioStatus)> failFunc;

    /// add a file load request to the queue
    void add(const URL& url, successFunc onSuccess, failFunc onFail=failFunc());
    /// add a file group request to the queue
    void addGroup(const Array<URL>& urls, groupSuccessFunc onSuccess, failFunc onFail=failFunc());
    /// update the queue, called per frame from runloop
    void update();
    /// get number of pending load actions
    int numPending() const;

    struct item {
        Ptr<IORead> ioRequest;
        successFunc onSuccess;
        failFunc onFail;
    };
    Array<item> items;
    struct groupItem {
        Array<Ptr<IORead>> ioRequests;
        groupSuccessFunc onSuccess;
        failFunc onFail;
    };
    Array<groupItem> groupItems;
};

} // namespace Oryol
