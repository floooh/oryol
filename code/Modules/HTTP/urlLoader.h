#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HTTP::urlLoader
    @brief private: processes a HTTP request and returns HTTP response    
    @see HTTPClient
*/
#if ORYOL_OSX
#include "HTTP/osx/osxURLLoader.h"
namespace Oryol {
namespace HTTP {
class urlLoader : public osxURLLoader {};
}
}
#else
#include "HTTP/base/baseURLLoader.h"
namespace Oryol {
namespace HTTP {
class urlLoader : public baseURLLoader {};
}
}
#endif
