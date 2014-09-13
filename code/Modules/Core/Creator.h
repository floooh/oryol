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

#if defined(__GNUC__) && !defined(__clang__)
/** WORKAROUND for GCC bug 'lambda argument pack' **/
#define OryolClassCreator(TYPE) \
public:\
static std::function<Ptr<TYPE>()> Creator() {\
    return [] { return Create(); };\
};
#else
#define OryolClassCreator(TYPE) \
public:\
template<typename... ARGS> static std::function<Ptr<TYPE>()> Creator(ARGS... args) {\
  return [args...] { return Create(args...); };\
};
#endif

