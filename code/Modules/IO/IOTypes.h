#pragma once
//------------------------------------------------------------------------------
#include "Core/Types.h"
#include "Core/Containers/Map.h"
#include "Core/String/StringAtom.h"
#include "Core/String/String.h"
#include "Core/String/StringBuilder.h"
#include <functional>

namespace Oryol {

class FileSystemBase;

//------------------------------------------------------------------------------
/**
    @class Oryol::IOSetup
    @ingroup IO
    @brief configure the IO system
*/
class IOSetup {
public:
    /// initial assigns
    Map<String, String> Assigns;
    /// initial file systems
    Map<StringAtom, std::function<Ptr<FileSystemBase>()>> FileSystems;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::IOStatus
    @ingroup IO
    @brief IO request status codes
    
    IO request status codes, these are simply HTTP status codes.
*/
class IOStatus {
public:
    /// status code enum
    enum Code {
        // 1xx Informational
        Continue = 100,
        SwitchingProtocols = 101,
        
        // 2xx Success
        OK = 200,
        Created = 201,
        Accepted = 202,
        NonAuthorativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        
        // 3xx Redirection
        MultipleChoices = 300,
        MovedPermanently = 301,
        Found = 302,
        SeeOther = 303,
        NotModified = 304,
        UseProxy = 305,
        TemporaryRedirect = 306,
        
        // 4xx Client Error
        BadRequest = 400,
        Unauthorized = 401,
        PaymentRequired = 402,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        NotAcceptable = 406,
        ProxyAuthenticationRequired = 407,
        RequestTimeout = 408,
        Conflict = 409,
        Gone = 410,
        LengthRequired = 411,
        PreconditionFailed = 412,
        RequestEntityTooLarge = 413,
        RequestURITooLarge = 414,
        UnsupportedMediaType = 415,
        RequestedRangeNotSatisfiable = 416,
        ExpectationFailed = 417,
        
        // 5xx Server Error
        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnavailable = 503,
        GatewayTimeout = 504,
        HTTPVersionNotSupported = 505,
        
        // these are custom Oryol status codes
        Cancelled = 1000,
        DownloadError = 1001,
        
        InvalidIOStatus = InvalidIndex
    };
    
    /// convert to string
    static const char* ToString(Code c);
};


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

//------------------------------------------------------------------------------
/**
    @class Oryol::URLBuilder
    @ingroup IO
    @brief build and manipulate URLs
        
    This allows to build or rebuild a URL from its parts. Constructing
    from an URL or calling the ParseURL() method will parse the URL parts
    into the URLBuilder. After that call the various setter methods
    to manipulate the URL parts, and call Build() to build a new 
    URL from them. Alternatively, build a URL from scratch by setting
    all required URL parts on an default-constructed URLBuilder
    and create the URL by calling the Build() method
 
    @see URL
*/
class URLBuilder {
public:
    /// default constructor
    URLBuilder();
    /// construct from URL
    URLBuilder(const URL& url);

    /// setup parts from URL
    void ParseURL(const URL& url);
    /// build URL from parts
    URL BuildURL();
    
    /// the scheme URL part (e.g. "http")
    String Scheme;
    /// the URL user part
    String User;
    /// the URL password part (clear-text!)
    String Password;
    /// the URL host-address part
    String Host;
    /// the port-number part
    String Port;
    /// local-path part
    String Path;
    /// the query part as key/value map
    Map<String, String> Query;
    /// the fragment part
    String Fragment;
    
private:
    StringBuilder stringBuilder;
};

} // namespace Oryol
