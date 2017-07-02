#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::baseURLLoader
    @ingroup _priv
    @brief base class for platform specific URL loaders
    @see urlLoader, HTTPClient
*/
#include "IO/private/ioRequests.h"

namespace Oryol {
namespace _priv {

class baseURLLoader {
public:
    /// process one HTTPRequest
    bool doRequest(const Ptr<IORead>& ioRequest);
};
} // namespace _priv
} // namespace Oryol