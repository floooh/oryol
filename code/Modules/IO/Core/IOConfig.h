#pragma once
//------------------------------------------------------------------------------
/**
    @file IOConfig.h
    @ingroup IO
    @brief compile-time configuration settings for IO system
*/
#include "Core/Types.h"

namespace Oryol {

class IOConfig {
public:
    /// number of IO workers (== number of HTTP connections)
    static const int NumWorkers = 4;
};

} // namespace Oryol
