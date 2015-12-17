#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::hostFileAccess
    @ingroup _priv
    @brief private: processes file system io
    @see HostFileSystem
*/

#if ORYOL_WINDOWS
#include "IO/HostFS/windows/winFileAccess.h"
namespace Oryol {
namespace _priv {
class hostFileAccess : public winFileAccess {};
}}
#elif ORYOL_POSIX
#include "IO/HostFS/posix/posixFileAccess.h"
namespace Oryol {
namespace _priv {
class hostFileAccess : public posixFileAccess {};
}}
#else
#include "IO/HostFS/base/baseFileAccess.h"
namespace Oryol {
namespace _priv {
class hostFileAccess : public baseFileAccess {};
}}
#endif
