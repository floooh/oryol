#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Singleton.h
    @brief class annotation macros for singletons
    
    These are class annotation macros to mark a C++ class as singleton.
    'Normal' Oryol singletons are thread-local. A singleton that's
    unique and visible in the whole process is a 'Global Singleton'.
*/
#include "Core/Types.h"
#include "Core/Assert.h"

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
