#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::fsWrapper
    @ingroup _priv
    @brief file-system wrapper frontend class
*/

// FIXME: add more platform-specific implementations?
#if ORYOL_POSIX || ORYOL_WINDOWS
#include "LocalFS/posix/posixFSWrapper.h"
namespace Oryol {
namespace _priv {
class fsWrapper : public posixFSWrapper { };
} }
#else
#include "LocalFS/dummy/dummyFSWrapper.h"
namespace Oryol {
namespace _priv {
class fsWrapper : public dummyFSWrapper { };
} }
#endif
