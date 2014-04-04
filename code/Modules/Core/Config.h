#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Config.h
    @brief global configuration defines
*/

// does the platform have threading support?
#if ORYOL_EMSCRIPTEN
#define ORYOL_HAS_THREADS (0)
#else
/// set to (1) on platform with pthreads-style threading support
#define ORYOL_HAS_THREADS (1)
#endif

#if ORYOL_WINDOWS
#define ORYOL_THREAD_LOCAL __declspec(thread)
#else
#define ORYOL_THREAD_LOCAL __thread
#endif

// platform specific max-alignment
#if ORYOL_EMSCRIPTEN
#define ORYOL_MAX_PLATFORM_ALIGN (4)
#else
/// max memory alignment for this platform
#define ORYOL_MAX_PLATFORM_ALIGN (16)
#endif

/// memory debug fill pattern (byte)
#define ORYOL_MEMORY_DEBUG_BYTE (0xBB)
/// memory debug fill pattern (short)
#define ORYOL_MEMORY_DEBUG_SHORT (0xBBBB)
/// memory debug fill pattern (int)
#define ORYOL_MEMORY_DEBUG_INT (0xBBBBBBBB)

/// minimum grow size for dynamic container classes (num elements)
#define ORYOL_CONTAINER_DEFAULT_MIN_GROW (16)
/// maximum grow size for dynamic container classes (num elements)
#define ORYOL_CONTAINER_DEFAULT_MAX_GROW (1<<16)

#ifndef __GNUC__
#define __attribute__(x)
#endif

/// silence unused variable warning
#define ORYOL_UNUSED __attribute__((unused))
