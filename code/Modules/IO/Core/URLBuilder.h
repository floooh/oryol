#pragma once
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
#include "IO/Core/URL.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
    
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