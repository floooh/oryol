#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::OpenMode
    @brief IO StreamBuffer open modes

    StreamBuffers can be opened as read-only, write-only, write-append, 
    or read/write:
    
    * ReadOnly: only reading is allowed, read-cursor is set to beginning when opened
    * WriteOnly: only writing is allowed, write-cursor will be set to 
                 beginning, and any current content will be cleared/overwritten
    * WriteAppend: only writing is allowed, write-cursor will be set to end of 
                   stream content
    * ReadWrite: read and write is allowed, content is erased and both read-
                 and write-cursor are set to position 0
    * ReadWriteAppend: like WriteAppend, but reading is allowed, read-cursor
                       will be set to position 0, write cursor to end of content
*/
#include "Core/Types.h"

namespace Oryol {
namespace IO {

class OpenMode {
public:
    /// enum values
    enum Enum {
        ReadOnly = 0,       //> open as read-only
        WriteOnly,          //> open in write-only mode, overwrite content
        WriteAppend,        //> open in write-append mode, append content
        ReadWrite,          //> open in read/write mode
        ReadWriteAppend,    //> open in read/write-append mode
        
        InvalidOpenMode     //> invalid open mode
    };

    /// convert OpenMode to string
    static const char* ToString(OpenMode::Enum m);
    /// convert OpenMode from string
    static OpenMode::Enum FromString(const char* str);
};

} // namespace IO
} // namespace Oryol