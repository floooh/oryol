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
    /// process one HTTPRequest
    bool doRequest(const Ptr<HTTPProtocol::HTTPRequest>& req);
};
} // namespace _priv
} // namespace Oryol