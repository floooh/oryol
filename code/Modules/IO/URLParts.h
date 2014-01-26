#pragma once
//------------------------------------------------------------------------------
/**
    @class IO::URLParts
    
    FIXME: ditch this method
 
    Holds the split parts that make up an URL.
 
    An URL consists of the following parts:
    
    scheme://user:pwd@host:port/path?queryKey=queryValue&...#fragment
*/
#include "Core/Types.h"
#include "Core/String/StringUtil.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace IO {

class URLParts
{
public:
    /// constructor
    URLParts();
    
    /// valid flag
    bool Valid;
    /// the URL scheme (http, file, ftp, ...)
    Core::String Scheme;
    /// the optional user name
    Core::String User;
    /// the optional password
    Core::String Password;
    /// the host address
    Core::String Host;
    /// the optional port number
    Core::String Port;
    /// the path component
    Core::String Path;
    /// the optional fragment
    Core::String Fragment;
    /// the query component
    Core::Map<Core::String,Core::String> Query;
};

//------------------------------------------------------------------------------
inline
URLParts::URLParts() :
Valid(false)
{
    // empty
}

} // namespace IO
} // namespace Oryol
