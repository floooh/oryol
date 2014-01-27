#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::Mode
    @brief IO StreamBuffer open modes

    StreamBuffers can be opened as read-only, write-only or read/write.
*/
#include "Core/Types.h"

namespace Oryol {
namespace IO {

enum class Mode {
    ReadOnly,   /// open as read-only
    WriteOnly,  /// open in write-only mode
    ReadWrite,  /// open in read/write mode
    
    InvalidMode /// invalid mode
};

} // namespace IO
} // namespace Oryol