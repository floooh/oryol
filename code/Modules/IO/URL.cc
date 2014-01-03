//------------------------------------------------------------------------------
//  URL.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "URL.h"
#include "Core/String/StringUtil.h"

namespace Oryol {
namespace IO {

using namespace String;
    
//------------------------------------------------------------------------------
URLParts
URL::Split() const {
    
    URLParts parts;
    std::string str = this->content.AsString();
    if (!str.empty()) {
        // FIXME FIXME FIXME: first resolve assigns
//        std::string str = assignRegistry::Instance()->ResolveAssigns(this->content);
        
        // extract scheme
        auto schemeAndRest = StringUtil::Bisect(str, ":/");
        if (schemeAndRest.second.empty()) {
            // URL has no scheme, not a valid URL
            parts.Valid = false;
            return parts;
        }
        parts.Scheme = schemeAndRest.first;

        // split into host and path part
        str = schemeAndRest.second;
        auto hostAndPath = StringUtil::Bisect(str, "/");
        if (hostAndPath.first.empty()) {
            // URL malformed, only consisted of scheme
            parts.Valid = false;
            return parts;
        }
        
        // split into user+pwd and host+port
        auto userPwdAndHost = StringUtil::Bisect(hostAndPath.first, "@");
        std::string host;
        if (!userPwdAndHost.second.empty()) {
            // split userPwd into user and pwd
            auto userAndPwd = StringUtil::Bisect(userPwdAndHost.first, ":");
            parts.User = userAndPwd.first;
            parts.Password = userAndPwd.second;
            host = userPwdAndHost.second;
        }
        else {
            host = userPwdAndHost.first;
        }
        
        // split host part into host and port number
        auto hostAndPort = StringUtil::Bisect(host, ":");
        parts.Host = hostAndPort.first;
        parts.Port = hostAndPort.second;
        
        if (!hostAndPath.second.empty()) {

            // split right-part into pathAndQuery and fragment
            auto pathQueryAndFragment = StringUtil::Bisect(hostAndPath.second, "#");
            if (!pathQueryAndFragment.second.empty()) {
                parts.Fragment = pathQueryAndFragment.second;
            }
            
            // split path and query
            auto pathAndQuery = StringUtil::Bisect(pathQueryAndFragment.first, "?");
            parts.Path = pathAndQuery.first;
            
            // extract query
            if (!pathAndQuery.second.empty()) {
                std::vector<std::string> tokens = StringUtil::Tokenize(pathAndQuery.second, "&;");
                for (const auto& token : tokens) {
                    auto keyValuePair = StringUtil::Bisect(token, "=");
                    parts.Query.insert(keyValuePair);
                }
            }
        }
        parts.Valid = true;
    }
    return parts;
}
    
} // namespace IO
} // namespace Oryol