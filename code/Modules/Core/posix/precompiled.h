#pragma once
//------------------------------------------------------------------------------
/**
    @file core/posix/precompiled.h
    
    Standard includes for POSIX platforms. 
    NOTE: keep as many headers out of here as possible, at least
    on compilers which don't have pre-compiled-headers turned on.
*/
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include <cstddef>
