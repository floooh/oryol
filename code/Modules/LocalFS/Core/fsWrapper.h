#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::fsWrapper
    @ingroup _priv
    @brief file-system wrapper frontend class
*/

#if ORYOL_EMSCRIPTEN
#include "LocalFS/dummy/dummyFSWrapper.h"
namespace Oryol {
namespace _priv {
class fsWrapper : public dummyFSWrapper { };
} }
#else
#include "LocalFS/posix/posixFSWrapper.h"
namespace Oryol {
namespace _priv {
class fsWrapper : public posixFSWrapper { };
} }
#endif
