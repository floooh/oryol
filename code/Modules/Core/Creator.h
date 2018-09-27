#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Creator.h
    @brief class annotation macro for Creator method
    
    A creator object is a function object for deferred object creation.
    The class annotation macro OryolClassCreator adds a static method 
    Creator(), which returns a creator object for that class.
*/
#include "Core/Ptr.h"
#include <functional>

#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 7 || (__GNUC__ == 7 && __GNUC_MINOR__ < 4) || (__GNUC__ == 8 && __GNUC_MINOR__ <1))
/** WORKAROUND for GCC bug 'lambda argument pack' **/
#define OryolClassCreator(TYPE) \
public:\
static Ptr<TYPE>(*Creator())() {\
    return [] { return Create(); };\
};
#else
#define OryolClassCreator(TYPE) \
public:\
template<typename... ARGS> static Ptr<TYPE>(*Creator(ARGS... args))() {\
  return [args...] { return Create(args...); };\
};
#endif

