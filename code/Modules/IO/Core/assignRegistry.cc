//------------------------------------------------------------------------------
//  assignRegistry.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "assignRegistry.h"
#include "Core/String/StringBuilder.h"

#if ORYOL_HAS_THREADS
#include <mutex>
static std::mutex lockMutex;
#define SCOPED_LOCK std::lock_guard<std::mutex> lock(lockMutex)
#else
#define SCOPED_LOCK
#endif

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
assignRegistry::SetAssign(const String& assign, const String& path) {
    SCOPED_LOCK;
    o_assert_dbg(assign.Back() == ':'); // "assign must end with a ':'\n"
    o_assert_dbg(assign.Length() > 1);  // assigns must be at least 2 chars to not be confused with DOS drive letters
    o_assert_dbg(!path.Empty());
    o_assert_dbg((path.Back() == '/') || (path.Back() == ':')); // path must end in a '/' (dir) or ':' (other assign)
    if (this->assigns.Contains(assign)) {
        this->assigns[assign] = path;
    }
    else {
        this->assigns.Add(assign, path);
    }
}

//------------------------------------------------------------------------------
bool
assignRegistry::HasAssign(const String& assign) const {
    SCOPED_LOCK;
    o_assert_dbg(!assign.Empty());
    bool result = this->assigns.Contains(assign);
    return result;
}

//------------------------------------------------------------------------------
String
assignRegistry::LookupAssign(const String& assign) const {
    SCOPED_LOCK;
    o_assert_dbg(!assign.Empty());
    o_assert_dbg(assign.Back() == ':');
    String result;
    if (this->assigns.Contains(assign)) {
        result = this->assigns[assign];
    }
    return result;
}

//------------------------------------------------------------------------------
String
assignRegistry::ResolveAssigns(const String& str) const {
    SCOPED_LOCK;
    StringBuilder builder;
    builder.Set(str);
    
    // while there are assigns to replace...
    int index;
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
    return result;
}

} // namespace _priv
} // namespace Oryol