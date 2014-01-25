#pragma once
//------------------------------------------------------------------------------
/**
    @class IO::URLParts
    
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
    
    /// set host and port (as 'host[:port]')
    void SetAddress(const Core::String& s);
    /// get host and port
    Core::String GetAddress() const;
};

//------------------------------------------------------------------------------
inline
URLParts::URLParts() :
Valid(false)
{
    // empty
}

//------------------------------------------------------------------------------
inline void
URLParts::SetAddress(const Core::String& addr) {
    Core::StringUtil::Bisect(addr, ":", this->Host, this->Port);
}

//------------------------------------------------------------------------------
inline Core::String
URLParts::GetAddress() const {
    if (!this->Port.Empty()) {
        return Core::StringUtil::Concatenate(':', {this->Host, this->Port});
    }
    else {
        return this->Host;
    }
}

} // namespace IO
} // namespace Oryol
