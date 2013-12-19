#pragma once
//------------------------------------------------------------------------------
/**
    @file core/macros.h

    Essential macro definitions for oryol.
*/
#include "core/types.h"
#include "core/log.h"

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
#define oryol_class_decl(TYPE, POOL_SIZE) \
private:\
    static oryol::memory::pool_allocator<TYPE, POOL_SIZE> allocator;\
public:\
    static int32 get_pool_size() {\
        return allocator.get_pool_size();\
    }\
    template<typename... ARGS> static TYPE* create(ARGS&&... args) {\
        return allocator.create(std::forward<ARGS>(args)...);\
    };\
    virtual void destroy() {\
        allocator.destroy(this);\
    };\
private:

#define oryol_class_impl(TYPE, POOL_SIZE) \
oryol::memory::pool_allocator<TYPE, POOL_SIZE> TYPE::allocator;

// global singleton macros (visible in all threads)
#define oryol_decl_global_singleton(TYPE) \
private:\
    static TYPE* singleton;\
public:\
    static TYPE* instance() {\
        o_assert(nullptr != singleton);\
        return singleton;\
    };\
    static bool has_instance() {\
        return nullptr != singleton;\
    };\
    static void create_singleton() {\
        o_assert(0 == singleton);\
        singleton = new TYPE();\
    };\
    static void destroy_singleton() {\
        o_assert(0 != singleton);\
        delete singleton;\
        singleton = 0;\
    };\
    void singleton_ensure_unique() {\
        o_assert(0 == singleton);\
    };\
private:

#define oryol_impl_global_singleton(TYPE) TYPE* TYPE::singleton = 0;

// thread-local singleton macros
#define oryol_decl_local_singleton(TYPE) \
private:\
    static __thread TYPE* singleton;\
public:\
    static TYPE* instance() {\
        o_assert(nullptr != singleton);\
        return singleton;\
    };\
    static bool has_instance() {\
        return nullptr != singleton;\
    };\
    static void create_singleton() {\
        o_assert(0 == singleton);\
        singleton = new TYPE();\
    };\
    static void destroy_singleton() {\
        o_assert(0 != singleton);\
        delete singleton;\
        singleton = 0;\
    };\
    void singleton_ensure_unique() {\
        o_assert(0 == singleton);\
    };\
private:

#define oryol_impl_local_singleton(TYPE) __thread TYPE* TYPE::singleton = 0;


