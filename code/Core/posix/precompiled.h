#pragma once
//------------------------------------------------------------------------------
/**
    @class core/posix/precompiled.h
    
    Standard includes for POSIX platforms.
*/
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif
    
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <memory>
#include <cstring>
#include <cwchar>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <type_traits>
#include <algorithm>
#if ORYOL_HAS_THREADS
#include <atomic>
#include <thread>
#include <mutex>
#endif

