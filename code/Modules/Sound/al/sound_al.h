#pragma once
//------------------------------------------------------------------------------
/**
    OpenAL header wrapper for Sound module
*/
#if ORYOL_OSX || ORYOL_IOS
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#else
#include "AL/al.h"
#include "AL/alc.h"
#endif

#if ORYOL_DEBUG
#define ORYOL_SOUND_AL_CHECK_ERROR() o_assert(alGetError() == AL_NO_ERROR)
#else
#define ORYOL_SOUND_AL_CHECK_ERROR() ((void)0)
#endif