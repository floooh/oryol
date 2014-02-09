#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Logger
    @brief base class for implementing custom loggers

    Derive from this class and override its methods in order to implement
    a custom logging functionality.
    
    @see Log
*/
#include "Core/Types.h"
#include "Core/Macros.h"
#include "Core/Log.h"
#include "Core/RefCounted.h"

namespace Oryol {
namespace Core {

class Logger : public RefCounted {
    OryolClassDecl(Logger);
public:
    /// constructor
    Logger();
    /// destructor
    virtual ~Logger();
    /// generic vprint-style method
    virtual void VPrint(Log::Level l, const char* msg, va_list args);
    /// print an assert message
    virtual void AssertMsg(const char* cond, const char* msg, const char* file, int32 line, const char* func);
};
    
} // namespace Core
} // namespace Oryol
