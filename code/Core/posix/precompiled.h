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
#include <vector>
#include <cstring>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <type_traits>
#if ORYOL_HAS_THREADS
#include <atomic>
#include <thread>
#include <mutex>
#endif

