#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::baseURLLoader
    @ingroup _priv
    @brief private: base class for platform specific URL loaders
    @see urlLoader, HTTPClient
*/
#include "Core/Types.h"
#include "Core/Containers/Queue.h"
#include "HTTP/HTTPProtocol.h"

namespace Oryol {
namespace _priv {

class baseURLLoader {
public:
    /// enqueue an URL request
    void putRequest(const Ptr<HTTPProtocol::HTTPRequest>& req);
    /// process enqueued requests
    void doWork();
protected:
    Queue<Ptr<HTTPProtocol::HTTPRequest>> requestQueue;
};
} // namespace _priv
} // namespace Oryol