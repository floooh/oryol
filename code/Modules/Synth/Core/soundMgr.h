#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::soundMgr
    @ingroup _priv
    @brief internal sound manager wrapper class
*/
#if (ORYOL_OSX || ORYOL_EMSCRIPTEN)
#include "Synth/al/alSoundMgr.h"
namespace Oryol {
namespace _priv {
class soundMgr : public alSoundMgr { };
} }
#else
#include "Synth/base/soundMgrBase.h"
namespace Oryol {
namespace _priv {
class soundMgr : public soundMgrBase { };
} }
#endif