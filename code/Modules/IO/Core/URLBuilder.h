#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::URLBuilder
    @ingroup IO
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
    void SetScheme(const String& scheme);
    /// get scheme URL part
    const String& GetScheme() const;
    /// set user URL part
    void SetUser(const String& user);
    /// get user URL part
    const String& GetUser() const;
    /// set password URL part
    void SetPassword(const String& pwd);
    /// get password URL part
    const String& GetPassword() const;
    /// set host URL part
    void SetHost(const String& host);
    /// get host URL part
    const String& GetHost() const;
    /// set port URL part
    void SetPort(const String& port);
    /// get port URL part
    const String& GetPort() const;
    /// set path URL part
    void SetPath(const String& path);
    /// get path URL part
    const String& GetPath() const;
    /// set query URL part
    void SetQuery(const Map<String, String>& query);
    /// get query URL part
    const Map<String, String>& GetQuery() const;
    /// set fragment URL part
    void SetFragment(const String& frag);
    /// get fragment URL part
    const String& GetFragment() const;
    
private:
    String scheme;
    String user;
    String password;
    String host;
    String port;
    String path;
    String fragment;
    Map<String, String> query;
    StringBuilder stringBuilder;
    
};
    
//------------------------------------------------------------------------------
inline void
URLBuilder::SetScheme(const String& s) {
    this->scheme = s;
}

//------------------------------------------------------------------------------
inline const String&
URLBuilder::GetScheme() const {
    return this->scheme;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetUser(const String& s) {
    this->user = s;
}

//------------------------------------------------------------------------------
inline const String&
URLBuilder::GetUser() const {
    return this->user;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetPassword(const String& s) {
    this->password = s;
}

//------------------------------------------------------------------------------
inline const String&
URLBuilder::GetPassword() const {
    return this->password;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetHost(const String& s) {
    this->host = s;
}

//------------------------------------------------------------------------------
inline const String&
URLBuilder::GetHost() const {
    return this->host;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetPort(const String& s) {
    this->port = s;
}

//------------------------------------------------------------------------------
inline const String&
URLBuilder::GetPort() const {
    return this->port;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetPath(const String& s) {
    this->path = s;
}

//------------------------------------------------------------------------------
inline const String&
URLBuilder::GetPath() const {
    return this->path;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetQuery(const Map<String, String>& q) {
    this->query = q;
}

//------------------------------------------------------------------------------
inline const Map<String, String>&
URLBuilder::GetQuery() const {
    return this->query;
}

//------------------------------------------------------------------------------
inline void
URLBuilder::SetFragment(const String& s) {
    this->fragment = s;
}

//------------------------------------------------------------------------------
inline const String&
URLBuilder::GetFragment() const {
    return this->fragment;
}

} // namespace Oryol