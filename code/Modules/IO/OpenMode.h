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
#include "Core/Macros.h"

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
        
        Invalid             //> invalid mode
    };

    /// convert OpenMode to string
    static const char* ToString(OpenMode::Enum m) {
        switch (m) {
            case OpenMode::ReadOnly:        return "ReadOnly";
            case OpenMode::WriteOnly:       return "WriteOnly";
            case OpenMode::WriteAppend:     return "WriteAppend";
            case OpenMode::ReadWrite:       return "ReadWrite";
            case OpenMode::ReadWriteAppend: return "ReadWriteAppend";
            default:                        return "Invalid";
        }
    };
    /// convert OpenMode from string
    static OpenMode::Enum FromString(const char* str) {
        o_assert(str);
        if (std::strcmp(str, "ReadOnly") == 0) return OpenMode::ReadOnly;
        else if (std::strcmp(str, "WriteOnly") == 0) return OpenMode::WriteOnly;
        else if (std::strcmp(str, "WriteAppend") == 0) return OpenMode::WriteAppend;
        else if (std::strcmp(str, "ReadWrite") == 0) return OpenMode::ReadWrite;
        else if (std::strcmp(str, "ReadWriteAppend") == 0) return OpenMode::ReadWriteAppend;
        else return OpenMode::Invalid;
    };
};

} // namespace IO
} // namespace Oryol