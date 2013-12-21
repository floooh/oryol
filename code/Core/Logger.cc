//------------------------------------------------------------------------------
//  Logger.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Logger.h"
#include "Core/Log.h"

namespace Oryol {
namespace Core {

OryolClassImpl(Logger, 1);

//------------------------------------------------------------------------------
Logger::Logger() {
    // empty
}

//------------------------------------------------------------------------------
Logger::~Logger() {
    // empty
}

//------------------------------------------------------------------------------
void
Logger::VPrint(Log::Level /*l*/, const char* /*msg*/, va_list /*args*/) {
    // we can't do an o_error() here since it would recurse
}

//------------------------------------------------------------------------------
/**
 */
void
Logger::AssertMsg(const char* /*cond*/, const char* /*msg*/, const char* /*file*/, int32 /*line*/, const char* /*func*/) {
    // we can't do an o_error() here since it would recurse
}

} // namespace Core
} // namespace Oryol