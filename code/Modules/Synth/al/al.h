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