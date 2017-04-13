#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::urlLoader
    @ingroup _priv
    @brief private: processes a HTTP request and returns HTTP response    
    @see HTTPClient
*/
#if ORYOL_USE_LIBCURL
#include "HTTP/curl/curlURLLoader.h"
namespace Oryol {
namespace _priv {
class urlLoader : public curlURLLoader {};
} }
#elif ORYOL_OSX
#include "HTTP/osx/osxURLLoader.h"
namespace Oryol {
namespace _priv {
class urlLoader : public osxURLLoader {};
} }
#elif ORYOL_WINDOWS
#include "HTTP/windows/winURLLoader.h"
namespace Oryol {
namespace _priv {
class urlLoader : public winURLLoader {};
} }
#elif ORYOL_EMSCRIPTEN
#include "HTTP/emsc/emscURLLoader.h"
namespace Oryol {
namespace _priv {
class urlLoader : public emscURLLoader {};
} }
#else
#include "HTTP/base/baseURLLoader.h"
namespace Oryol {
namespace _priv {
class urlLoader : public baseURLLoader {};
} }
#endif
