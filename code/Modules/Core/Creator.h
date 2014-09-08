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

#if !ORYOL_ANDROID
#define OryolClassCreator(TYPE) \
public:\
template<typename... ARGS> static std::function<Core::Ptr<TYPE>()> Creator(ARGS... args) {\
  return [args...] { return Create(args...); };\
};
#else
// the Android NDK gcc version currently doesn't compile lambdas with parameter packs
#define OryolClassCreator(TYPE) \
public:\
static std::function<Core::Ptr<TYPE>()> Creator() {\
    return [] { return Create(); };\
};
#endif

