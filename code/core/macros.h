#pragma once
//------------------------------------------------------------------------------
/**
    @file core/macros.h

    Essential macro definitions for oryol.
*/
#include "core/types.h"

#if ORYOL_WINDOWS
#define ORYOL_TRAP __debugbreak
#else
#define ORYOL_TRAP __builtin_trap
#endif

// assert macros
#if (__clang_analyzer__ || ORYOL_USE_STDASSERT)
    #include <assert.h>
    #define o_assert(cond) assert(cond)
    #define o_assert2(cond, msg) assert(cond)
#elif ORIOL_NO_ASSERT
    #define o_assert(cond) ((void)0)
    #define o_assert2(cond, msg) ((void)0)
#else
    #if ORYOL_POSIX
        #define o_assert(cond) do { if(!(cond)) { oryol::core::log::assert_msg(#cond,nullptr,__FILE__,__LINE__,__PRETTY_FUNCTION__); ORYOL_TRAP(); } } while(0)
        #define o_assert2(cond, msg) do { if(!(cond)) { oryol::core::log::assert_msg(#cond,msg,__FILE__,__LINE__,__PRETTY_FUNCTION__); ORYOL_TRAP(); } while(0)
        #define o_error(...) do { oryol::log::error(__VA_ARGS__); ORYOL_TRAP(); } while(0)
    #else
        #define o_assert(cond) do { if(!(cond)) { oryol::core::log::assert_msg(#cond,nullptr__FILE__,__LINE__,__FUNCSIG__); ORYOL_TRAP(); } } while(0)
        #define o_assert2(cond, msg) do { if(!(cond)) { oryol::core::log::assert_msg(#cond,msg,__FILE__,__LINE__,__FUNCSIG__); ORYOL_TRAP(); } } while (0)
        #define o_error(...) do { oryol::log::error(__VA_ARGS__); ORYOL_TRAP(); } while(0)
    #endif
#endif

// add oryol standard methods to a class
#define oryol_class(TYPE) \
public:\
template<typename... ARGS> static std::shared_ptr<TYPE> create_shared(ARGS&&... args) {\
    return std::make_shared<TYPE>(std::forward<ARGS>(args)...);\
};\
template<typename... ARGS> static std::unique_ptr<TYPE> create_unique(ARGS&&... args) {\
    return std::unique_ptr<TYPE>(new TYPE(std::forward<ARGS>(args)...));\
};\
private:
