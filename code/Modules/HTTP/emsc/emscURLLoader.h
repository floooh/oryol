#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HTTP::emscURLLoader
    @brief private: emscripten implementation of URL loader
    @see URLLoader, HTTPClient

    A emscURLLoader will process http requests sequentially. In order to
    handle several requests in parallel, create and feed several
    URLLoaders in parallel.
*/
#include "HTTP/base/baseURLLoader.h"

namespace Oryol {
namespace HTTP {

class emscURLLoader : public baseURLLoader {
public:
    /// process enqueued requests
    void doWork();

private:
    /// start the next, called from doWork
    void startRequest(const Core::Ptr<HTTPProtocol::HTTPRequest>& req);
    /// success callback
    static void onLoaded(void* userData, void* buffer, int size);
    /// failure callback
    static void onFailed(void* userData);
};
    
} // namespace HTTP 
} // namespace Oryol
