#pragma once
//------------------------------------------------------------------------------
/**
    @file core/windows/precompiled.h
    
    Standard includes for Windows platform.
*/
#include <cstddef>

#ifdef _MSC_VER
#if _MSC_VER < 1900
#error "Oryol no longer supports VS2013, please switch to VS2015 and use win64-vs2015-xxx fips build configs"
#endif
#endif