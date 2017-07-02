#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::urlLoader
    @ingroup _priv
    @brief processes a HTTP request and returns HTTP response    
    @see HTTPClient
*/
#if ORYOL_USE_LIBCURL
#include "HttpFS/private/curl/curlURLLoader.h"
namespace Oryol {
namespace _priv {
class urlLoader : public curlURLLoader {};
} }
#elif ORYOL_OSX
#include "HttpFS/private/osx/osxURLLoader.h"
namespace Oryol {
namespace _priv {
class urlLoader : public osxURLLoader {};
} }
#elif ORYOL_WINDOWS
#include "HttpFS/private/win/winURLLoader.h"
namespace Oryol {
namespace _priv {
class urlLoader : public winURLLoader {};
} }
#elif ORYOL_EMSCRIPTEN
#include "HttpFS/private/emsc/emscURLLoader.h"
namespace Oryol {
namespace _priv {
class urlLoader : public emscURLLoader {};
} }
#else
#include "HttpFS/private/baseURLLoader.h"
namespace Oryol {
namespace _priv {
class urlLoader : public baseURLLoader {};
} }
#endif
