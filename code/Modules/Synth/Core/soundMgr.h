#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::soundMgr
    @brief internal sound manager wrapper class
*/
#if (ORYOL_OSX || ORYOL_EMSCRIPTEN)
#include "Synth/al/alSoundMgr.h"
namespace Oryol {
namespace Synth {
class soundMgr : public alSoundMgr { };
} }
#else
#include "Synth/base/soundMgrBase.h"
namespace Oryol {
namespace Synth {
class soundMgr : public soundMgrBase { };
} }
#endif