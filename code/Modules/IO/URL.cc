//------------------------------------------------------------------------------
//  URL.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "URL.h"
#include "Core/String/StringUtil.h"
#include "Core/Log.h"
#include "IO/assignRegistry.h"

namespace Oryol {
namespace IO {

using namespace Core;
    
//------------------------------------------------------------------------------
URLParts
URL::Split() const {
    
    URLParts parts;
    String str = this->content.AsString();
    if (!str.Empty()) {
        if (assignRegistry::HasInstance()) {
            str = assignRegistry::Instance()->ResolveAssigns(str);
        }
        else {
            Log::Warn("URL::Split(): no assignRegistry singleton!\n");
        }
        
        // extract scheme
        String rightOfScheme;
        StringUtil::Bisect(str, ":/", parts.Scheme, rightOfScheme);
        if (parts.Scheme.Empty()) {
            // URL has no scheme, not a valid URL
            parts.Valid = false;
            return parts;
        }

        // split into host and path part
        String hostAll;
        String pathAll;
        StringUtil::Bisect(str, "/", hostAll, pathAll);
        if (hostAll.Empty()) {
            // URL malformed, only consisted of scheme
            parts.Valid = false;
            return parts;
        }
        
        // split into user+pwd and host+port
        String hostUserPwd;
        String hostAddrPort;
        StringUtil::Bisect(hostAll, "@", hostUserPwd, hostAddrPort);
        if (!hostAddrPort.Empty()) {
            // split userPwd into user and pwd
            StringUtil::Bisect(hostUserPwd, ":", parts.User, parts.Password);
        }
        else {
            hostAddrPort = hostUserPwd;
        }
        
        // split host part into host and port number
        StringUtil::Bisect(hostAddrPort, ":", parts.Host, parts.Port);
        
        if (!pathAll.Empty()) {

            // split right-part into pathAndQuery and fragment
            String pathAndQuery;
            StringUtil::Bisect(pathAll, "#", pathAndQuery, parts.Fragment);
            
            // split path and query
            String query;
            StringUtil::Bisect(pathAndQuery, "?", parts.Path, query);
            
            // extract query
            if (!query.Empty()) {
                Array<String> tokens;
                StringUtil::Tokenize(query, "&;", tokens);
                for (const String& token : tokens) {
                    String key, value;
                    StringUtil::Bisect(token, "=", key, value);
                    parts.Query.Insert(key, value);
                }
            }
        }
        parts.Valid = true;
    }
    return parts;
}
    
//------------------------------------------------------------------------------
URL
URL::Build(const URLParts& parts) {
o_error("FIXME!");
return URL();
/*
    o_assert(parts.Valid);
    std::string str = parts.Scheme;
    str.append("://");
    if (!parts.User.empty()) {
        str.append(parts.User);
        if (!parts.Password.empty()) {
            str.push_back(':');
            str.append(parts.Password);
        }
        str.push_back('@');
    }
    str.append(parts.Host);
    if (!parts.Port.empty()) {
        str.push_back(':');
        str.append(parts.Port);
    }
    str.push_back('/');
    str.append(parts.Path);
    if (!parts.Query.empty()) {
        str.push_back('?');
        for (const auto& kvp : parts.Query) {
            str.append(kvp.first);
            str.push_back('=');
            str.append(kvp.second);
            str.push_back('&');
        }
        str.pop_back();
    }
    if (!parts.Fragment.empty()) {
        str.push_back('#');
        str.append(parts.Fragment);
    }
    return URL(str);
*/
}

} // namespace IO
} // namespace Oryol