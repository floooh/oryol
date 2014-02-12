//------------------------------------------------------------------------------
//  assignRegistry.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "assignRegistry.h"
#include "Core/String/StringBuilder.h"

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
    o_assert(assign.Back() == ':'); // "assign must end with a ':'\n"
    o_assert(assign.Length() > 1);  // assigns must be at least 2 chars to not be confused with DOS drive letters
    o_assert(!path.Empty());
    o_assert((path.Back() == '/') || (path.Back() == ':')); // path must end in a '/' (dir) or ':' (other assign)
    
    this->rwLock.LockWrite();
    if (this->assigns.Contains(assign)) {
        this->assigns[assign] = path;
    }
    else {
        this->assigns.Insert(assign, path);
    }
    this->rwLock.UnlockWrite();
}

//------------------------------------------------------------------------------
bool
assignRegistry::HasAssign(const String& assign) const {
    o_assert(!assign.Empty());
    this->rwLock.LockRead();
    bool result = this->assigns.Contains(assign);
    this->rwLock.UnlockRead();
    return result;
}

//------------------------------------------------------------------------------
String
assignRegistry::LookupAssign(const String& assign) const {
    o_assert(!assign.Empty());
    o_assert(assign.Back() == ':');
    String result;
    this->rwLock.LockRead();
    if (this->assigns.Contains(assign)) {
        result = this->assigns[assign];
    }
    this->rwLock.UnlockRead();
    return result;
}

//------------------------------------------------------------------------------
String
assignRegistry::ResolveAssigns(const String& str) const {

    this->rwLock.LockRead();
    
    StringBuilder builder;
    builder.Set(str);
    
    // while there are assigns to replace...
    int32 index;
    while ((index = builder.FindFirstOf(0, EndOfString, ":")) != EndOfString) {
        // ignore DOS drive letters
        if (index > 1) {
            String assignString = builder.GetSubString(0, index + 1);

            // lookup the assign, ignore unknown assigns, may be URL schemes
            if (this->assigns.Contains(assignString)) {
                
                // replace assign string
                builder.SubstituteFirst(assignString, this->assigns[assignString]);
            }
            else break;
        }
        else break;
    }
    String result = builder.GetString();
    this->rwLock.UnlockRead();
    return result;
}

//------------------------------------------------------------------------------
void
assignRegistry::setStandardAssigns() {
    /// @todo: implement a platform-specific systemPaths class
    /// which obtains the systemPaths in a platform-specific way
}


} // namespace IO
} // namespace Oryol