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

#define OryolClassCreator(TYPE) \
public:\
template<typename... ARGS> static std::function<Core::Ptr<TYPE>(ARGS...)> Creator(ARGS... args) {\
  return [&, args...] { return Create(args...); };\
};

