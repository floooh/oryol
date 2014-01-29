#pragma once
//------------------------------------------------------------------------------
/**
    @file IO/Config.h
    @brief Configuration defines for the Oryol IO module.
*/
#include "Core/Config.h"

/// minimum grow size for streams (in bytes)
#define ORYOL_STREAM_DEFAULT_MIN_GROW (256)
/// maximum grow size for streams (in bytes)
#define ORYOL_STREAM_DEFAULT_MAX_GROW (1<<18)   // 256 kByte
