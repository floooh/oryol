#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pnaclURLLoader
    @ingroup _priv
    @brief private: PNaCl implementation of URLLoader
    @see URLLoader, HTTPClient

    See the url_loader example in the NaCl SDK for details.
*/
#include "HTTP/base/baseURLLoader.h"

namespace Oryol {
namespace _priv {
    
class pnaclURLLoader : public baseURLLoader {
public:
    /// process enqueued requests
    void doWork();

    /// pepper-thread callback which sends the request
    static void cbSendRequest(void* data, int32_t result);
    /// pepper-thread callback after HTTP response header has been loaded
    static void cbRequestComplete(void* data, int32_t result);
    /// pepper-thread callback to fetch response body
    static void cbOnRead(void* data, int32_t result);

private:
    /// start the next, called from doWork
    void startRequest(const Ptr<HTTPProtocol::HTTPRequest>& req);
};

} // namespace _priv
} // namespace Oryol
