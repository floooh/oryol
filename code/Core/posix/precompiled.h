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

