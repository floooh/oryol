#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Macros.h
    @brief essential macro definitions for Oryol
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
#elif ORIOL_NO_ASSERT
    #define o_assert(cond) ((void)0)
    #define o_assert2(cond, msg) ((void)0)
#else
    #if ORYOL_POSIX
        /// standard assert macro
        #define o_assert(cond) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,nullptr,__FILE__,__LINE__,__PRETTY_FUNCTION__); ORYOL_TRAP(); } } while(0)
        /// assert macro with programmer's message
        #define o_assert2(cond, msg) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,msg,__FILE__,__LINE__,__PRETTY_FUNCTION__); ORYOL_TRAP(); } while(0)
    #else
        #define o_assert(cond) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,nullptr,__FILE__,__LINE__,__FUNCSIG__); ORYOL_TRAP(); } } while(0)
        #define o_assert2(cond, msg) do { if(!(cond)) { Oryol::Core::Log::AssertMsg(#cond,msg,__FILE__,__LINE__,__FUNCSIG__); ORYOL_TRAP(); } } while (0)
    #endif
#endif

#if ORYOL_POSIX
    /// print a critical error and abort execution
    #define o_error(...) do { Oryol::Core::Log::Error(__VA_ARGS__); ORYOL_TRAP(); } while(0)
#else
    #define o_error(...) do { Oryol::Core::Log::Error(__VA_ARGS__); ORYOL_TRAP(); } while(0)
#endif

/// declare an Oryol class with pool allocator (located inside class declaration)
#define OryolClassPoolAllocDecl(TYPE) \
private:\
    static Oryol::Core::poolAllocator<TYPE> allocator;\
protected:\
    virtual void destroy() {\
        TYPE::allocator.Destroy(this);\
    };\
public:\
    template<typename... ARGS> static TYPE* Create(ARGS&&... args) {\
        return TYPE::allocator.Create(std::forward<ARGS>(args)...);\
    };\

/// implementation-side macro for Oryol class with pool allocator (located in .cc source file)
#define OryolClassPoolAllocImpl(TYPE) \
Oryol::Core::poolAllocator<TYPE> TYPE::allocator;

/// implementation-side macro for template classes with pool allocator (located in .cc source file)
#define OryolTemplClassPoolAllocImpl(TEMPLATE_TYPE, CLASS_TYPE) \
template<class TEMPLATE_TYPE> Oryol::Core::poolAllocator<CLASS_TYPE<TEMPLATE_TYPE>> CLASS_TYPE<TEMPLATE_TYPE>::allocator;

/// declare an oryol class without pool allocator (located inside class declaration)
#define OryolClassDecl(TYPE) \
protected:\
    virtual void destroy() {\
        delete(this);\
    };\
public:\
    template<typename... ARGS> static TYPE* Create(ARGS&&... args) {\
        return new TYPE(std::forward<ARGS>(args)...);\
    };\

/// implementation-side macro for Oryol class without pool allocator (located in .cc source file)
#define OryolClassImpl(TYPE)

/// global singleton declaration macros (visible in all threads, located in class declaration)
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

/// implementation-side macro for global singleton (located in .cc source file)
#define OryolGlobalSingletonImpl(TYPE) TYPE* TYPE::singleton = 0;

/// thread-local singleton declaration macro (located inside class declaration)
#define OryolLocalSingletonDecl(TYPE) \
private:\
    static ORYOL_THREAD_LOCAL TYPE* singleton; \
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

/// implementation-side macrot for thread-local singletons (located in .cc source file)
#define OryolLocalSingletonImpl(TYPE) ORYOL_THREAD_LOCAL TYPE* TYPE::singleton = 0;


