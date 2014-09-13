#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::winURLLoader
    @ingroup _priv
    @brief private: Windows implementation of URLLoader
    @see urlLoader HTTPClient

    The winURLLoader will keep an internal pool of open HTTP connections
    which will expire after a couple of seconds.
 */
#include "HTTP/base/baseURLLoader.h"
#include "HTTP/HTTPProtocol.h"
#include "Core/String/StringBuilder.h"
#include <chrono>

// don't include windows headers here
typedef void* HINTERNET;

namespace Oryol {
namespace _priv {
    
class winURLLoader : public baseURLLoader {
public:
    /// constructor, calls InternetOpen()
    winURLLoader();
    /// destructor, calls InternetCloseHandle()
    ~winURLLoader();
    /// process enqueued requests
    void doWork();

private:
    /// process a single HTTP request
    void doOneRequest(const Ptr<HTTPProtocol::HTTPRequest>& req);
    /// open connection, or get already open connection
    HINTERNET obtainConnection(const URL& url);
    /// garbage collect expired connections (called from doWork)
    void garbageCollectConnections();

    static const std::chrono::seconds connectionMaxAge;   // disconnect after 10 seconds
    const String contentTypeString;
    HINTERNET hSession;
    struct connection {
        HINTERNET hConnection;
        std::chrono::steady_clock::time_point timeStamp;
    };
    Map<String, connection> connections;
    StringBuilder stringBuilder;
};
    
} // namespace _priv
} // namespace Oryol