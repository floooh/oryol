#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::soundMgr
    @ingroup _priv
    @brief top-level sound manager platform-wrapper
*/
#if ORYOL_OPENAL
#include "Sound/al/alSoundMgr.h"
namespace Oryol {
namespace _priv {
class soundMgr : public alSoundMgr { };
} }
#else
namespace Oryol {
namespace _priv {
class soundMgr : public soundMgrBase { };
} }
#endif