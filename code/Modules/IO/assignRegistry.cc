//------------------------------------------------------------------------------
//  assignRegistry.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "assignRegistry.h"

namespace Oryol {
namespace IO {

OryolGlobalSingletonImpl(assignRegistry);

//------------------------------------------------------------------------------
assignRegistry::assignRegistry() {
    this->SingletonEnsureUnique();
    this->setStandardAssigns();
}

//------------------------------------------------------------------------------
assignRegistry::~assignRegistry() {
    // empty
}

//------------------------------------------------------------------------------
void
assignRegistry::SetAssign(const std::string& assign, const std::string& path) {
    o_assert(!assign.empty());
    o_assert(assign.length() > 1);  // assigns must be at least 2 chars to not be confused with DOS drive letters
    o_assert(!path.empty());
    std::lock_guard<std::mutex> lock(this->mtx);
    this->assigns[assign] = path;
}

//------------------------------------------------------------------------------
bool
assignRegistry::HasAssign(const std::string& assign) const {
    o_assert(!assign.empty());
    std::lock_guard<std::mutex> lock(this->mtx);
    return this->assigns.find(assign) != this->assigns.end();
}

//------------------------------------------------------------------------------
std::string
assignRegistry::LookupAssign(const std::string& assign) const {
    o_assert(!assign.empty());
    std::lock_guard<std::mutex> lock(this->mtx);
    auto iter = this->assigns.find(assign);
    if (iter != this->assigns.end()) {
        return iter->second;
    }
    else {
        return std::string();
    }
}

//------------------------------------------------------------------------------
std::string
assignRegistry::ResolveAssigns(const std::string& str) const {
    std::lock_guard<std::mutex> lock(this->mtx);
    
    std::string result = str;
    std::string::size_type index;
    while ((index = result.find_first_of(':', 0)) != std::string::npos) {
        // ignore DOS drive letters
        if (index > 1) {
            std::string assignString(result, 0, index);

            // lookup the assign, ignore unknown assigns, may be URL schemes
            auto iter = this->assigns.find(assignString);
            if (iter != this->assigns.end()) {
            
                // get the string after the colon
                std::string postAssignString;
                auto postAssignIndex = index + 1;
                if (postAssignIndex < result.size()) {
                    postAssignString.assign(result, postAssignIndex, std::string::npos);
                }
                
                // build the new string
                result = iter->second;
                if (result.back() != ':' && result.back() != '/') {
                    result.push_back('/');
                }
                result.append(postAssignString);
            }
            else break;
        }
        else break;
    }
    if (result.back() == '/') {
        result.pop_back();
    }
    return result;
}

//------------------------------------------------------------------------------
void
assignRegistry::setStandardAssigns() {
    // FIXME: implement a platform-specific systemPaths class
    // which obtains the systemPaths in a platform-specific way
}


} // namespace IO
} // namespace Oryol