#pragma once
//------------------------------------------------------------------------------
/**
    OpenAL header wrapper.
*/
#if ORYOL_OSX || ORYOL_IOS
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#else
#include "AL/al.h"
#include "AL/alc.h"
#endif

// Oryol AL error checking macro
#if ORYOL_DEBUG
#define ORYOL_AL_CHECK_ERROR() o_assert(alGetError() == AL_NO_ERROR)
#else
#define ORYOL_AL_CHECK_ERROR() ((void)0)
#endif