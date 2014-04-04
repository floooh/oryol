#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Assert.h
    @brief Oryol's assert macros
*/
#include "Core/Types.h"
#include "Core/Log.h"

#if ORYOL_WINDOWS
#define ORYOL_TRAP __debugbreak
#else
/// macro to stop execution and enter debugger
#define ORYOL_TRAP __builtin_trap
#endif

// assert macros
#if (__clang_analyzer__ || ORYOL_USE_STDASSERT)
#include <assert.h>
#define o_assert(cond) assert(cond)
#define o_assert2(cond, msg) assert(cond)
#define o_assert_dbg(cond) assert(cond)
#define o_assert2_dbg(cond, msg) assert(cond)
#define o_assert_range(val, max) assert((val >= 0) && (val < max))
#define o_assert_range_dbg(val, max) assert((val >= 0) && (val < max))
#elif !ORYOL_DEBUG && ORYOL_NO_ASSERT
#define o_assert(cond) ((void)0)
#define o_assert2(cond, msg) ((void)0)
#define o_assert_range(val, max) ((void)0)
#define o_assert_dbg(cond) ((void)0)
#define o_assert2_dbg(cond, msg) ((void)0)
#define o_assert_range_dbg(val, max) ((void)0)
#else
/// standard assert macro
#define o_assert(cond) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,nullptr,__FILE__,__LINE__,__PRETTY_FUNCTION__); ORYOL_TRAP(); } } while(0)
/// assert macro which is only active in debug mode
#if ORYOL_DEBUG
#define o_assert_dbg(cond) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,nullptr,__FILE__,__LINE__,__PRETTY_FUNCTION__); ORYOL_TRAP(); } } while(0)
#else
#define o_assert_dbg(cond) ((void)0)
#endif
/// assert macro with programmer's message
#define o_assert2(cond, msg) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,msg,__FILE__,__LINE__,__PRETTY_FUNCTION__); ORYOL_TRAP(); } } while(0)
#if ORYOL_DEBUG
#define o_assert2_dbg(cond, msg) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,msg,__FILE__,__LINE__,__PRETTY_FUNCTION__); ORYOL_TRAP(); } } while(0)
#else
#define o_assert2_dbg(cond, msg) ((void)0)
#endif
/// perform a range check (x >= 0) && (x < max)
#define o_assert_range(val, max) o_assert((val >= 0) && (val < max))
#define o_assert_range_dbg(val, max) o_assert_dbg((val >= 0) && (val < max))
#endif

#if ORYOL_POSIX
/// print a critical error and abort execution
#define o_error(...) do { Oryol::Core::Log::Error(__VA_ARGS__); ORYOL_TRAP(); } while(0)
#else
#define o_error(...) do { Oryol::Core::Log::Error(__VA_ARGS__); ORYOL_TRAP(); } while(0)
#endif

