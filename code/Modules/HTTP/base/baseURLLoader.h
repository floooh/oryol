#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HTTP::baseURLLoader
    @brief private: base class for platform specific URL loaders
    @see urlLoader, HTTPClient
*/
#include "Core/Types.h"
#include "Core/Containers/Queue.h"
#include "HTTP/HTTPProtocol.h"

namespace Oryol {
namespace HTTP {

class baseURLLoader {
public:
    /// enqueue an URL request
    void putRequest(const Core::Ptr<HTTPProtocol::HTTPRequest>& req);
    /// process enqueued requests
    void doWork();
protected:
    Core::Queue<Core::Ptr<HTTPProtocol::HTTPRequest>> requestQueue;
};
} // namespace HTTP
} // namespace Oryol