#pragma once
//------------------------------------------------------------------------------
/**
    @class Core/precompiled.h
    
    Standard system includes for various platforms.
*/
#include "Core/Config.h"
#if ORYOL_POSIX
#include "Core/posix/precompiled.h"
#elif ORYOL_WINDOWS
#include "Core/windows/precompiled.h"
#else
#error "Invalid platform!"
#endif
