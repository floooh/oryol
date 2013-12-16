#pragma once
//------------------------------------------------------------------------------
/**
    @class core/precompiled.h
    
    Standard system includes for various platforms.
*/
#include "core/config.h"
#if ORYOL_POSIX
#include "core/posix/precompiled.h"
#else
#error "Invalid platform!"
#endif
