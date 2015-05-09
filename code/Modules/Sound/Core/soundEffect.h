#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::soundEffect
    @ingroup _priv
    @brief sound effect resource wrapper class
*/
#if ORYOL_OPENAL
#include "Sound/al/alSoundEffect.h"
namespace Oryol {
namespace _priv {
class soundEffect : public alSoundEffect { };
} }
#else
#include "Sound/Core/soundEffectBase.h"
namespace Oryol {
namespace _priv {
class soundEffect : public soundEffectBase { };
} }
#endif
