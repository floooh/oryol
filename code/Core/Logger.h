#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Logger
    
    Base class for loggers. Create logger objects and add them to
    Oryol::Core::Log.
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
