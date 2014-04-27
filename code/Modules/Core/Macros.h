#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Macros.h
    @brief essential macro definitions for Oryol
*/
#include "Core/Types.h"

/// declare an Oryol class with pool allocator (located inside class declaration)
#define OryolClassPoolAllocDecl(TYPE) \
private:\
    static Oryol::Core::poolAllocator<TYPE> allocator;\
protected:\
    virtual void destroy() {\
        TYPE::allocator.Destroy(this);\
    };\
public:\
    template<typename... ARGS> static Core::Ptr<TYPE> Create(ARGS&&... args) {\
        return Core::Ptr<TYPE>(TYPE::allocator.Create(std::forward<ARGS>(args)...));\
    };\

/// implementation-side macro for Oryol class with pool allocator (located in .cc source file)
#define OryolClassPoolAllocImpl(TYPE) \
Oryol::Core::poolAllocator<TYPE> TYPE::allocator;

/// implementation-side macro for template classes with pool allocator (located in .cc source file)
#define OryolTemplClassPoolAllocImpl(TEMPLATE_TYPE, CLASS_TYPE) \
template<class TEMPLATE_TYPE> Oryol::Core::poolAllocator<CLASS_TYPE<TEMPLATE_TYPE>> CLASS_TYPE<TEMPLATE_TYPE>::allocator;

/// declare an Oryol class without pool allocator (located inside class declaration)
#define OryolClassDecl(TYPE) \
protected:\
    virtual void destroy() {\
        delete(this);\
    };\
public:\
    template<typename... ARGS> static Core::Ptr<TYPE> Create(ARGS&&... args) {\
        return Core::Ptr<TYPE>(new TYPE(std::forward<ARGS>(args)...));\
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
    template<typename... ARGS> static TYPE* CreateSingle(ARGS&&... args) {\
        o_assert(0 == singleton);\
        singleton = new TYPE(std::forward<ARGS>(args)...);\
        return singleton;\
    };\
    static void DestroySingle() {\
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
    template<typename... ARGS> static TYPE* CreateSingle(ARGS&&... args) {\
        o_assert(0 == singleton);\
        singleton = new TYPE(std::forward<ARGS>(args)...);\
        return singleton; \
    };\
    static void DestroySingle() {\
        o_assert(0 != singleton);\
        delete singleton;\
        singleton = 0;\
    };\
    void SingletonEnsureUnique() {\
        o_assert(0 == singleton);\
    };\
private:

/// implementation-side macro for thread-local singletons (located in .cc source file)
#define OryolLocalSingletonImpl(TYPE) ORYOL_THREAD_LOCAL TYPE* TYPE::singleton = 0;

/// to-string / from-string helper macros
#define __ORYOL_FROMSTRING(code) if (0 == std::strcmp(#code, str)) return code
#define __ORYOL_TOSTRING(code) case code: return #code
