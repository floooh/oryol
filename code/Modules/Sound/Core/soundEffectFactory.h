#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::soundEffectFactory
    @ingroup _priv
    @brief platform-wrapper-frontend for sound effect factory
*/
#if ORYOL_OPENAL
#include "Sound/al/alSoundEffectFactory.h"
namespace Oryol {
namespace _priv {
class soundEffectFactory : public alSoundEffectFactory { };
} }
#else
#include "Sound/Core/soundEffectFactoryBase.h"
namespace Oryol {
namespace _priv {
class soundEffectFactory : public soundEffectFactoryBase { };
} }
#endif
