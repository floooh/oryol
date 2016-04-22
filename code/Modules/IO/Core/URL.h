#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::URL
    @ingroup IO
    @brief Oryol's URL class
    
    All resource paths in Oryol are expressed as URLs. 
    On creation and assignment, the URL will be
    parsed and indices to its parts will be stored internally, this
    is quite fast. The actual URL string will be stored as a StringAtom.
    Expensive String construction only happens when actually getting
    the URL parts. 
    
    @see URLBuilder
*/
#include "Core/Types.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/Map.h"
#include "Core/String/String.h"

namespace Oryol {

class URL
{
public:
    /// default constructor
    URL();
    /// copy constructor
    URL(const URL& rhs);
    /// move constructor
    URL(URL&& rhs);
    /// construct from raw string
    URL(const char* rhs);
    /// construct from StringAtom
    URL(const StringAtom& rhs);
    /// construct from String
    URL(const String& rhs);
    
    /// assignment from other URL
    void operator=(const URL& rhs);
    /// move-assign from other URL
    void operator=(URL&& rhs);
    /// assign from raw string
    void operator=(const char* rhs);
    /// assignment from StringAtom
    void operator=(const StringAtom& rhs);
    /// assignment from String
    void operator=(const String& rhs);
    
    /// equality
    bool operator==(const URL& rhs);
    /// inequality
    bool operator!=(const URL& rhs);
    
    /// return true if this is a valid, non-empty URL
    bool IsValid() const;
    /// return true if the URL is empty
    bool Empty() const;
    /// get the URL string
    const StringAtom& Get() const;
    /// shortcut: get content as C string
    const char* AsCStr() const;
    
    /// test if the URL has a scheme
    bool HasScheme() const;
    /// get the scheme string
    String Scheme() const;
    /// test if the URL has a user string
    bool HasUser() const;
    /// get the user string
    String User() const;
    /// test if the URL has a password
    bool HasPassword() const;
    /// get the password string
    String Password() const;
    /// test if the URL has a host string
    bool HasHost() const;
    /// get the host string
    String Host() const;
    /// test if the URL has a port string
    bool HasPort() const;
    /// get the port string
    String Port() const;
    /// get host and port (only host if no port was in URL)
    String HostAndPort() const;
    /// test if the URL has a path string
    bool HasPath() const;
    /// get the path string
    String Path() const;
    /// test if the URL has a fragment
    bool HasFragment() const;
    /// get the fragment string
    String Fragment() const;
    /// test if the URL has a query
    bool HasQuery() const;
    /// get the query component
    Map<String, String> Query() const;
    /// get everything right of the server
    String PathToEnd() const;
    
private:
    /// crack URL, populates string indices
    void crack(String urlString);
    /// clear string indices
    void clearIndices();
    /// copy string indices
    void copyIndices(const URL& rhs);
    
    enum {
        schemeStart = 0,
        schemeEnd,
        userStart,
        userEnd,
        pwdStart,
        pwdEnd,
        hostStart,
        hostEnd,
        portStart,
        portEnd,
        pathStart,
        pathEnd,
        fragStart,
        fragEnd,
        queryStart,
        queryEnd,
        
        numIndices,
    };
    
    StringAtom content;
    int16_t indices[numIndices];
    bool valid;
};
   
} // namespace Oryol