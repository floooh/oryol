#pragma once
//------------------------------------------------------------------------------
/**
    @file core/posix/precompiled.h
    
    Standard includes for POSIX platforms. 
    NOTE: keep as many headers out of here as possible, at least
    on compilers which don't have pre-compiled-headers turned on.
*/

// this is a workaround when using clang with the GNU std lib,
// this fails without __STRICT_ANSI__ because clang doesn't
// know the __float128 type
#if __clang__ && ORYOL_LINUX && !defined(__STRICT_ANSI__)
#define __STRICT_ANSI__
#endif

#include <cstddef>
