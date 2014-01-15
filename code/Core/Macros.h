#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Macros.h

    Essential macro definitions for oryol.
*/
#include "Core/Types.h"
#include "Core/Log.h"

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
        #define o_assert(cond) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,nullptr,__FILE__,__LINE__,__PRETTY_FUNCTION__); ORYOL_TRAP(); } } while(0)
        #define o_assert2(cond, msg) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,msg,__FILE__,__LINE__,__PRETTY_FUNCTION__); ORYOL_TRAP(); } while(0)
        #define o_error(...) do { Oryol::Core::Log::Error(__VA_ARGS__); ORYOL_TRAP(); } while(0)
    #else
        #define o_assert(cond) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,nullptr__FILE__,__LINE__,__FUNCSIG__); ORYOL_TRAP(); } } while(0)
        #define o_assert2(cond, msg) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,msg,__FILE__,__LINE__,__FUNCSIG__); ORYOL_TRAP(); } } while (0)
        #define o_error(...) do { oryol::log::error(__VA_ARGS__); ORYOL_TRAP(); } while(0)
    #endif
#endif

// declare an oryol class with pool allocator
#define OryolClassPoolAllocDecl(TYPE, POOL_SIZE) \
private:\
    static Oryol::Memory::poolAllocator<TYPE, POOL_SIZE> allocator;\
protected:\
    virtual void destroy() {\
        TYPE::allocator.Destroy(this);\
    };\
public:\
    static int32 GetPoolSize() {\
        return TYPE::allocator.GetPoolSize();\
    }\
    template<typename... ARGS> static TYPE* Create(ARGS&&... args) {\
        return TYPE::allocator.Create(std::forward<ARGS>(args)...);\
    };\

#define OryolClassPoolAllocImpl(TYPE, POOL_SIZE) \
Oryol::Memory::poolAllocator<TYPE, POOL_SIZE> TYPE::allocator;

// declare an oryol class without pool allocator
#define OryolClassDecl(TYPE) \
protected:\
virtual void destroy() {\
    delete(this);\
};\
public:\
static int32 GetPoolSize() {\
    return 0;\
}\
template<typename... ARGS> static TYPE* Create(ARGS&&... args) {\
    return new TYPE(std::forward<ARGS>(args)...);\
};\

#define OryolClassImpl(TYPE)

// global singleton macros (visible in all threads)
#define OryolGlobalSingletonDecl(TYPE) \
private:\
    static TYPE* singleton;\
public:\
    static TYPE* Instance() {\
        o_assert(nullptr != singleton);\
        return singleton;\
    };\
    static bool HasInstance() {\
        return nullptr != singleton;\
    };\
    static void CreateSingleton() {\
        o_assert(0 == singleton);\
        singleton = new TYPE();\
    };\
    static void DestroySingleton() {\
        o_assert(0 != singleton);\
        delete singleton;\
        singleton = 0;\
    };\
    void SingletonEnsureUnique() {\
        o_assert(0 == singleton);\
    };\
private:

#define OryolGlobalSingletonImpl(TYPE) TYPE* TYPE::singleton = 0;

// thread-local singleton macros
#define OryolLocalSingletonDecl(TYPE) \
private:\
    static __thread TYPE* singleton;\
public:\
    static TYPE* Instance() {\
        o_assert(nullptr != singleton);\
        return singleton;\
    };\
    static bool HasInstance() {\
        return nullptr != singleton;\
    };\
    static void CreateSingleton() {\
        o_assert(0 == singleton);\
        singleton = new TYPE();\
    };\
    static void DestroySingleton() {\
        o_assert(0 != singleton);\
        delete singleton;\
        singleton = 0;\
    };\
    void SingletonEnsureUnique() {\
        o_assert(0 == singleton);\
    };\
private:

#define OryolLocalSingletonImpl(TYPE) __thread TYPE* TYPE::singleton = 0;


