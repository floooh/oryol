//------------------------------------------------------------------------------
//  assignRegistry.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "assignRegistry.h"
#include "Core/String/StringUtil.h"

namespace Oryol {
namespace IO {

OryolGlobalSingletonImpl(assignRegistry);

using namespace Core;

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
assignRegistry::SetAssign(const String& assign, const String& path) {
    o_assert(!assign.Empty());
    o_assert(assign.Length() > 1);  // assigns must be at least 2 chars to not be confused with DOS drive letters
    o_assert(!path.Empty());
    std::lock_guard<std::mutex> lock(this->mtx);
    if (this->assigns.Contains(assign)) {
        this->assigns[assign] = path;
    }
    else {
        this->assigns.Insert(assign, path);
    }
}

//------------------------------------------------------------------------------
bool
assignRegistry::HasAssign(const String& assign) const {
    o_assert(!assign.Empty());
    std::lock_guard<std::mutex> lock(this->mtx);
    return this->assigns.Contains(assign);
}

//------------------------------------------------------------------------------
String
assignRegistry::LookupAssign(const String& assign) const {
    o_assert(!assign.Empty());
    std::lock_guard<std::mutex> lock(this->mtx);
    if (this->assigns.Contains(assign)) {
        return this->assigns[assign];
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
String
assignRegistry::ResolveAssigns(const String& str) const {

    // FIXME: this should be optimized:
    //  - resolve assigns when adding to the registry, so that
    //    an actual resolve doesn't need to loop
    //  - reduce number of string operations...
    
    // FIXME: use string builder!

    std::lock_guard<std::mutex> lock(this->mtx);
    
    String result = str;
    int32 index;
    while ((index = StringUtil::FindFirstOf(result, 0, ":")) != InvalidIndex) {
        // ignore DOS drive letters
        if (index > 1) {
            String assignString(result, 0, index);

            // lookup the assign, ignore unknown assigns, may be URL schemes
            if (this->assigns.Contains(assignString)) {
            
                // get the string after the colon
                String postAssignString;
                int32 postAssignIndex = index + 1;
                if (postAssignIndex < result.Length()) {
                    postAssignString.Assign(result, postAssignIndex, 0);
                }
                
                // build the new string
                String resolved = this->assigns[assignString];
                if (resolved.Back() != ':' && resolved.Back() != '/') {
                    result = StringUtil::Append({ resolved, postAssignString, "/" });
                }
                else {
                    result = StringUtil::Append({ resolved, postAssignString });
                }
            }
            else break;
        }
        else break;
    }
    if (result.Back() == '/') {
        result = StringUtil::Truncate(result, result.Length() - 1);
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