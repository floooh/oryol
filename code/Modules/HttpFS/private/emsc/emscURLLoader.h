#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::emscURLLoader
    @ingroup _priv
    @brief emscripten implementation of URL loader
    @see urlLoader
*/
#include "HttpFS/private/baseURLLoader.h"

namespace Oryol {
namespace _priv {

class emscURLLoader : public baseURLLoader {
public:
    /// process one request
    bool doRequest(const Ptr<IORead>& req);

    /// start the next, called from doWork
    void startRequest(const Ptr<IORead>& req);
    /// success callback
    static void onLoaded(void* userData, void* buffer, int size);
    /// failure callback
    static void onFailed(void* userData);
};

} // namespace _priv 
} // namespace Oryol
