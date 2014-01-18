#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Config.h
    
    Manage global configuration defines.
*/

// does the platform have threading support?
#if ORYOL_EMSCRIPTEN
#define ORYOL_HAS_THREADS (0)
#else
#define ORYOL_HAS_THREADS (1)
#endif

// platform specific max-alignment
#if ORYOL_EMSCRIPTEN
#define ORYOL_MAX_PLATFORM_ALIGN (4)
#else
#define ORYOL_MAX_PLATFORM_ALIGN (16)
#endif

// memory debug fill pattern
#define ORYOL_MEMORY_DEBUG_BYTE (0xBB)
#define ORYOL_MEMORY_DEBUG_SHORT (0xBBBB)
#define ORYOL_MEMORY_DEBUG_INT (0xBBBBBBBB)