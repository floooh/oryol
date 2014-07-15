#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::URLBuilder
    @brief build and manipulate URLs
        
    This allows to build or rebuild a URL from its parts. Constructing
    from an URL or calling the SetURL() method will parse the URL parts
    into the URLBuilder. After that call the various setter methods
    to manipulate the URL parts, and call Build() to build a new 
    URL from them. Alternatively, build a URL from scratch by setting
    all required URL parts on an default-constructed URLBuilder
    and create the URL by calling the Build() method
 
    @see URL
*/
#include "IO/Core/URL.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
namespace IO {
    
class URLBuilder {
public:
    /// default constructor
    URLBuilder();
    /// construct from URL
    URLBuilder(const URL& url);

    /// build URL from current parts
    URL BuildURL();
    
    /// set complete URL
    void SetURL(const URL& url);
    /// set scheme URL part (e.g. "http")
    void SetScheme(const Core::String& scheme);
    /// get scheme URL part
    const Core::String& GetScheme() const;
    /// set user URL part
    void SetUser(const Core::String& user);
    /// get user URL part
    const Core::String& GetUser() const;
    /// set password URL part
    void SetPassword(const Core::String& pwd);
    /// get password URL part
    const Core::String& GetPassword() const;
    /// set host URL part
    void SetHost(const Core::String& host);
    /// get host URL part
    const Core::String& GetHost() const;
    /// set port URL part
    void SetPort(const Core::String& port);
    /// get port URL part
    const Core::String& GetPort() const;
    /// set path URL part
    void SetPath(const Core::String& path);
    /// get path URL part
    const Core::String& GetPath() const;
    /// set query URL part
    void SetQuery(const Core::Map<Core::String, Core::String>& query);
    /// get query URL part
    const Core::Map<Core::String, Core::String>& GetQuery() const;
    /// set fragment URL part
    void SetFragment(const Core::String& frag);
    /// get fragment URL part
    const Core::String& GetFragment() const;
    
private:
    Core::String scheme;
    Core::String user;
    Core::String password;
    Core::String host;
    Core::String port;
    Core::String path;
    Core::String fragment;
    Core::Map<Core::String, Core::String> query;
    Core::StringBuilder stringBuilder;
    
};
    
//------------------------------------------------------------------------------
inline void
URLBuilder::SetScheme(const Core::String& s) {
    this->scheme = s;
}

//------------------------------------------------------------------------------
inline const Core::String&
URLBuilder::GetScheme() const {
    return this->scheme;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetUser(const Core::String& s) {
    this->user = s;
}

//------------------------------------------------------------------------------
inline const Core::String&
URLBuilder::GetUser() const {
    return this->user;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetPassword(const Core::String& s) {
    this->password = s;
}

//------------------------------------------------------------------------------
inline const Core::String&
URLBuilder::GetPassword() const {
    return this->password;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetHost(const Core::String& s) {
    this->host = s;
}

//------------------------------------------------------------------------------
inline const Core::String&
URLBuilder::GetHost() const {
    return this->host;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetPort(const Core::String& s) {
    this->port = s;
}

//------------------------------------------------------------------------------
inline const Core::String&
URLBuilder::GetPort() const {
    return this->port;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetPath(const Core::String& s) {
    this->path = s;
}

//------------------------------------------------------------------------------
inline const Core::String&
URLBuilder::GetPath() const {
    return this->path;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetQuery(const Core::Map<Core::String, Core::String>& q) {
    this->query = q;
}

//------------------------------------------------------------------------------
inline const Core::Map<Core::String, Core::String>&
URLBuilder::GetQuery() const {
    return this->query;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetFragment(const Core::String& s) {
    this->fragment = s;
}

//------------------------------------------------------------------------------
inline const Core::String&
URLBuilder::GetFragment() const {
    return this->fragment;
}

} // namespace IO
} // namespace Oryol