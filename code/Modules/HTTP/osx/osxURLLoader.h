#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HTTP::osxURLLoader
    @brief private: OSX implementation of URLLoader
    @see urlLoader, HTTPClient
*/
#include "HTTP/base/baseURLLoader.h"

namespace Oryol {
namespace HTTP {
    
class osxURLLoader : public baseURLLoader {
public:
    /// process enqueued requests
    void doWork();
    
private:
    /// handle a single request, synchronously
    void doOneRequest(const Core::Ptr<HTTPProtocol::HTTPRequest>& req);
};
    
} // namespace HTTP
} // namespace Oryol
