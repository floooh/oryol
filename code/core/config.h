#pragma once
//------------------------------------------------------------------------------
/**
    @file core/config.h
    
    Manage global configuration defines.
*/

// does the platform have threading support?
#if ORYOL_EMSCRIPTEN
#define ORYOL_HAS_THREADS (1)
#else
#define ORYOL_HAS_THREADS (1)
#endif
