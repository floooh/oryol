#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Config.h
    @brief global configuration defines
*/

// does the platform have std::thread support?
#if ORYOL_FORCE_NO_THREADS
#define ORYOL_HAS_THREADS (0)
#elif ORYOL_EMSCRIPTEN
#define ORYOL_HAS_THREADS (0)
#elif ORYOL_IOS
// iOS SDK doesn't support thread-local-storage as keyword and
// I don't want to go back to pthreads, so suck it iOS, no threads for you
#define ORYOL_HAS_THREADS (0)
#else
#define ORYOL_HAS_THREADS (1)
#endif

// does the platform have std::atomic support?
#define ORYOL_HAS_ATOMIC (1)

#if ORYOL_HAS_THREADS
    #if ORYOL_WINDOWS
    #define ORYOL_THREAD_LOCAL __declspec(thread)
    #else
    #define ORYOL_THREAD_LOCAL __thread
    #endif
#else
    #define ORYOL_THREAD_LOCAL
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
