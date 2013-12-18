#pragma once
//------------------------------------------------------------------------------
/**
    @class core/posix/precompiled.h
    
    Standard includes for POSIX platforms.
*/
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <vector>
#include <cstring>
#include <typeinfo>
#include <typeindex>
#include <string>
#if ORYOL_HAS_THREADS
#include <atomic>
#include <thread>
#endif

