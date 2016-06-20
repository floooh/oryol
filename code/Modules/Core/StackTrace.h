#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::StackTrace
    @brief low-level helper functions to dump a stack trace
*/
#include "Core/Types.h"

namespace Oryol {

class StackTrace {
public:
    /// write stack trace into buf as human-readable string 
    static void Dump(char* buf, int bufSize);
};

} // namespace Oryol
