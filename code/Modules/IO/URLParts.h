#pragma once
//------------------------------------------------------------------------------
/**
    @class IO::URLParts
    
    Holds the split parts that make up an URL. Use the URLUtil methods
    to split an URL into parts, and build URLs from parts.
    
    An URL consists of the following parts:
    
    scheme://user:pwd@host:port/path?queryKey=queryValue&...#fragment
*/
#include "Core/Types.h"
#include <map>

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
    std::string Scheme;
    /// the optional user name
    std::string User;
    /// the optional password
    std::string Password;
    /// the host address
    std::string Host;
    /// the optional port number
    std::string Port;
    /// the path component
    std::string Path;
    /// the optional fragment
    std::string Fragment;
    /// the query component
    std::map<std::string,std::string> Query;
    
    /// set host and port (as 'host[:port]')
    void SetAddress(const std::string& s);
    /// get host and port
    std::string GetAddress() const;
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
URLParts::SetAddress(const std::string& addr) {
    std::string::size_type pos = addr.find(':');
    if (std::string::npos != pos) {
        this->Host = addr.substr(0, pos);
        this->Port = addr.substr(pos + 1, std::string::npos);
    }
    else {
        // does not contains the [:port] part
        this->Host = addr;
    }
}

//------------------------------------------------------------------------------
inline std::string
URLParts::GetAddress() const {
    std::string addr = this->Host;
    if (!this->Port.empty()) {
        addr.append(1, ':');
        addr.append(this->Port);
    }
    return addr;
}

} // namespace IO
} // namespace Oryol
