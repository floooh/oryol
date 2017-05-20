#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Logger
    @ingroup Core
    @brief base class for implementing custom loggers

    Derive from this class and override its methods in order to implement
    a custom logging functionality.
    
    @see Log
*/
#include "Core/Log.h"
#include "Core/RefCounted.h"

namespace Oryol {

class Logger : public RefCounted {
    OryolClassDecl(Logger);
public:
    /// constructor
    Logger();
    /// destructor
    ~Logger();
    /// generic vprint-style method
    virtual void VPrint(Log::Level l, const char* msg, va_list args);
    /// print an assert message
    virtual void AssertMsg(const char* cond, const char* msg, const char* file, int line, const char* func);
};
    
} // namespace Oryol
