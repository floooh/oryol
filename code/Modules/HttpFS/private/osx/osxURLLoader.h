#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::osxURLLoader
    @ingroup _priv
    @brief OSX implementation of URLLoader
    @see urlLoader, HTTPClient
*/
#include "HttpFS/private/baseURLLoader.h"

namespace Oryol {
namespace _priv {
    
class osxURLLoader : public baseURLLoader {
public:
    /// constructor
    osxURLLoader();
    /// process one HTTPRequest
    bool doRequest(const Ptr<IORead>& ioRequest);

    /// handle a single request, synchronously
    void doRequestInternal(const Ptr<IORead>& ioRequest);
};
    
} // namespace _priv
} // namespace Oryol
