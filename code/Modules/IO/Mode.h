#pragma once
//------------------------------------------------------------------------------
/**
    @class IO::Mode
    
    Stream open mode.
*/
#include "Core/Types.h"

namespace Oryol {
namespace IO {

enum class Mode {
    ReadOnly,
    WriteOnly,
    ReadWrite,
    
    InvalidMode
};

} // namespace IO
} // namespace Oryol