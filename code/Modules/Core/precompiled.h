#pragma once
//------------------------------------------------------------------------------
/**
    @class Core/precompiled.h
    
    Standard system includes for various platforms.
*/
#include "Core/Config.h"
#if ORYOL_POSIX
#include "Core/private/posix/precompiled.h"
#elif ORYOL_WINDOWS
#include "Core/private/windows/precompiled.h"
#else
#error "Invalid platform!"
#endif
