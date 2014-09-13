#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::osxURLLoader
    @ingroup _priv
    @brief private: OSX implementation of URLLoader
    @see urlLoader, HTTPClient
*/
#include "HTTP/base/baseURLLoader.h"

namespace Oryol {
namespace _priv {
    
class osxURLLoader : public baseURLLoader {
public:
    /// process enqueued requests
    void doWork();
    
private:
    /// handle a single request, synchronously
    void doOneRequest(const Ptr<HTTPProtocol::HTTPRequest>& req);
};
    
} // namespace _priv
} // namespace Oryol
