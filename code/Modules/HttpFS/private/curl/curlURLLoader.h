#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::curlURLLoader
    @ingroup _priv
    @brief urlLoader implementation on top of curl
    @see urlLoader
*/
#include "HttpFS/private/baseURLLoader.h"

namespace Oryol {
namespace _priv {

class curlURLLoader : public baseURLLoader {
public:
    /// constructor
    curlURLLoader();
    /// destructor
    ~curlURLLoader();
    /// process one request
    bool doRequest(const Ptr<IORead>& req);

    /// setup curl session
    void setupCurlSession();
    /// discard the curl session
    void discardCurlSession();
    /// process one request (internal)
    void doRequestInternal(const Ptr<IORead>& req);
    /// curl write-data callback
    static size_t curlWriteDataCallback(char* ptr, size_t size, size_t nmemb, void* userData);
    /// curl header-data callback
    static size_t curlHeaderCallback(char* ptr, size_t size, size_t nmenb, void* userData);

    void* curlSession;
    char* curlError;
};

} // namespace _priv
} // namespace Oryol
