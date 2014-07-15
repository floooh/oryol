//------------------------------------------------------------------------------
//  URL.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "URL.h"
#include "Core/String/StringBuilder.h"
#include "Core/Log.h"
#include "IO/Core/assignRegistry.h"

namespace Oryol {
namespace IO {

using namespace Core;

//------------------------------------------------------------------------------
void
URL::clearIndices() {
    for (int i = 0; i < numIndices; i++) {
        this->indices[i] = InvalidIndex;
    }
}

//------------------------------------------------------------------------------
void
URL::copyIndices(const URL& rhs) {
    for (int i = 0; i < numIndices; i++) {
        this->indices[i] = rhs.indices[i];
    }
}

//------------------------------------------------------------------------------
URL::URL() :
valid(false) {
    this->clearIndices();
}
    
//------------------------------------------------------------------------------
URL::URL(const URL& rhs) :
content(rhs.content),
valid(rhs.valid) {
    this->copyIndices(rhs);
}
    
//------------------------------------------------------------------------------
URL::URL(URL&& rhs) {
    this->content = std::move(rhs.content);
    this->copyIndices(rhs);
    this->valid = rhs.valid;
    rhs.clearIndices();
    rhs.valid = false;
}
    
//------------------------------------------------------------------------------
URL::URL(const char* rhs) :
valid(false) {
    this->crack(rhs);
}
    
//------------------------------------------------------------------------------
URL::URL(const Core::StringAtom& rhs) :
valid(false) {
    this->crack(rhs.AsString());
}
    
//------------------------------------------------------------------------------
URL::URL(const Core::String& rhs) :
content(rhs),
valid(false) {
    this->crack(rhs);
}
    
//------------------------------------------------------------------------------
void
URL::operator=(const URL& rhs) {
    if (&rhs != this) {
        this->content = rhs.content;
        this->copyIndices(rhs);
        this->valid = rhs.valid;
    }
}
    
//------------------------------------------------------------------------------
void
URL::operator=(URL&& rhs) {
    if (&rhs != this) {
        this->content = rhs.content;
        this->copyIndices(rhs);
        this->valid = rhs.valid;
        rhs.content.Clear();
        rhs.clearIndices();
        rhs.valid = false;
    }
}
    
//------------------------------------------------------------------------------
void
URL::operator=(const char* rhs) {
    this->content = rhs;
    this->crack(rhs);
}
    
//------------------------------------------------------------------------------
void
URL::operator=(const Core::StringAtom& rhs) {
    this->content = rhs;
    this->crack(rhs.AsString());
}
    
//------------------------------------------------------------------------------
void
URL::operator=(const Core::String& rhs) {
    this->content = rhs;
    this->crack(rhs);
}
    
//------------------------------------------------------------------------------
bool
URL::operator==(const URL& rhs) {
    return this->content == rhs.content;
}
    
//------------------------------------------------------------------------------
bool
URL::operator!=(const URL& rhs) {
    return this->content != rhs.content;
}
    
//------------------------------------------------------------------------------
const Core::StringAtom&
URL::Get() const {
    return this->content;
}

//------------------------------------------------------------------------------
const char*
URL::AsCStr() const {
    return this->content.AsCStr();
}

//------------------------------------------------------------------------------
bool
URL::IsValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
bool
URL::Empty() const {
    return !this->content.IsValid();
}

//------------------------------------------------------------------------------
void
URL::crack(String urlString) {

    this->content.Clear();
    this->clearIndices();
    this->valid = false;
    
    if (assignRegistry::HasInstance()) {
        urlString = assignRegistry::Instance()->ResolveAssigns(urlString);
    }
    if (urlString.IsValid()) {
    
        StringBuilder builder;
        builder.Set(urlString);
        this->content = urlString;
        
        // extract scheme
        this->indices[schemeStart] = 0;
        this->indices[schemeEnd] = builder.FindSubString(0, 8, "://");
        if (EndOfString == this->indices[schemeEnd]) {
            Log::Warn("URL::crack(): '%s' is not a valid URL!\n", this->content.AsCStr());
            this->clearIndices();
            return;
        }
        
        // extract host fields
        int32 leftStartIndex = this->indices[schemeEnd] + 3;
        int32 leftEndIndex = builder.FindFirstOf(leftStartIndex, EndOfString, "/");
        if (EndOfString == leftEndIndex) {
            leftEndIndex = builder.Length();
        }
        if (leftStartIndex != leftEndIndex) {
            // extract user and password
            int32 userAndPwdEndIndex = builder.FindFirstOf(leftStartIndex, leftEndIndex, "@");
            if (EndOfString != userAndPwdEndIndex) {
                // only user, or user:pwd?
                int32 userEndIndex = builder.FindFirstOf(leftStartIndex, userAndPwdEndIndex, ":");
                if (EndOfString != userEndIndex) {
                    // user and password
                    this->indices[userStart] = leftStartIndex;
                    this->indices[userEnd]   = userEndIndex;
                    this->indices[pwdStart]  = userEndIndex + 1;
                    this->indices[pwdEnd]    = userAndPwdEndIndex;
                }
                else {
                    // only user
                    this->indices[userStart] = leftStartIndex;
                    this->indices[userEnd]   = userAndPwdEndIndex;
                }
                leftStartIndex = userAndPwdEndIndex + 1;
            }
            
            // extract host and port
            int32 hostEndIndex = builder.FindFirstOf(leftStartIndex, leftEndIndex, ":");
            if (EndOfString != hostEndIndex) {
                // host and port
                this->indices[hostStart] = leftStartIndex;
                this->indices[hostEnd]   = hostEndIndex;
                this->indices[portStart] = hostEndIndex + 1;
                this->indices[portEnd]   = leftEndIndex;
            }
            else {
                // only host name, no port
                this->indices[hostStart] = leftStartIndex;
                this->indices[hostEnd]   = leftEndIndex;
            }
        }
        
        // is there any path component?
        if (leftEndIndex != builder.Length()) {
            // extract right-hand-side (path, fragment, query)
            int32 rightStartIndex = leftEndIndex + 1;
            int32 rightEndIndex = builder.Length();
            
            int32 pathStartIndex = rightStartIndex;
            int32 pathEndIndex = builder.FindFirstOf(rightStartIndex, rightEndIndex, "#?");
            if (EndOfString == pathEndIndex) {
                pathEndIndex = rightEndIndex;
            }
            if (pathStartIndex != pathEndIndex) {
                this->indices[pathStart] = pathStartIndex;
                this->indices[pathEnd]   = pathEndIndex;
            }

            // extract query
            if ((pathEndIndex != rightEndIndex) && (builder.At(pathEndIndex) == '?')) {
                int32 queryStartIndex = pathEndIndex + 1;
                int32 queryEndIndex = builder.FindFirstOf(queryStartIndex, rightEndIndex, "#");
                if (EndOfString == queryEndIndex) {
                    queryEndIndex = rightEndIndex;
                }
                if (queryStartIndex != queryEndIndex) {
                    this->indices[queryStart] = queryStartIndex;
                    this->indices[queryEnd] = queryEndIndex;
                }
                pathEndIndex = queryEndIndex;
            }
            
            // extract fragment
            if ((pathEndIndex != rightEndIndex) && (builder.At(pathEndIndex) == '#')) {
                int32 fragStartIndex = pathEndIndex + 1;
                int32 fragEndIndex = builder.FindFirstOf(fragStartIndex, rightEndIndex, "?");
                if (EndOfString == fragEndIndex) {
                    fragEndIndex = rightEndIndex;
                }
                if (fragStartIndex != fragEndIndex) {
                    this->indices[fragStart] = fragStartIndex;
                    this->indices[fragEnd] = fragEndIndex;
                }
            }
        }
        this->valid = true;
    }
    // fallthrough if valid or empty URL
}

//------------------------------------------------------------------------------
bool
URL::HasScheme() const {
    return InvalidIndex != this->indices[schemeStart];
}

//------------------------------------------------------------------------------
String
URL::Scheme() const {
    if (this->HasScheme()) {
        return String(this->content.AsCStr(), this->indices[schemeStart], this->indices[schemeEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasUser() const {
    return InvalidIndex != this->indices[userStart];
}

//------------------------------------------------------------------------------
String
URL::User() const {
    if (this->HasUser()) {
        return String(this->content.AsCStr(), this->indices[userStart], this->indices[userEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasPassword() const {
    return InvalidIndex != this->indices[pwdStart];
}

//------------------------------------------------------------------------------
String
URL::Password() const {
    if (this->HasPassword()) {
        return String(this->content.AsCStr(), this->indices[pwdStart], this->indices[pwdEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasHost() const {
    return InvalidIndex != this->indices[hostStart];
}

//------------------------------------------------------------------------------
String
URL::Host() const {
    if (this->HasHost()) {
        return String(this->content.AsCStr(), this->indices[hostStart], this->indices[hostEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasPort() const {
    return InvalidIndex != this->indices[portStart];
}

//------------------------------------------------------------------------------
String
URL::Port() const {
    if (this->HasPort()) {
        return String(this->content.AsCStr(), this->indices[portStart], this->indices[portEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
String
URL::HostAndPort() const  {
    if (this->HasHost()) {
        if (this->HasPort()) {
            // URL has host and port definition
            return String(this->content.AsCStr(), this->indices[hostStart], this->indices[portEnd]);
        }
        else {
            // URL only has host
            return String(this->content.AsCStr(), this->indices[hostStart], this->indices[hostEnd]);
        }
    }
    else {
        // no host in URL
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasPath() const {
    return InvalidIndex != this->indices[pathStart];
}

//------------------------------------------------------------------------------
String
URL::Path() const {
    if (this->HasPath()) {
        return String(this->content.AsCStr(), this->indices[pathStart], this->indices[pathEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasFragment() const {
    return InvalidIndex != this->indices[fragStart];
}

//------------------------------------------------------------------------------
String
URL::Fragment() const {
    if (this->HasFragment()) {
        return String(this->content.AsCStr(), this->indices[fragStart], this->indices[fragEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
String
URL::PathToEnd() const {
    if (this->HasPath()) {
        return String(this->content.AsCStr(), this->indices[pathStart], EndOfString);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasQuery() const {
    return InvalidIndex != this->indices[queryStart];
}

//------------------------------------------------------------------------------
Map<String, String>
URL::Query() const {
    if (this->HasQuery()) {
        Map<String, String> query;
        StringBuilder builder;
        builder.Set(this->content.AsCStr(), this->indices[queryStart], this->indices[queryEnd]);
        int32 kvpStartIndex = 0;
        int32 kvpEndIndex = 0;
        do {
            kvpEndIndex = builder.FindFirstOf(kvpStartIndex, EndOfString, "&");
            int32 keyEndIndex = builder.FindFirstOf(kvpStartIndex, kvpEndIndex, "=");
            if (EndOfString != keyEndIndex) {
                // key and value
                String key(builder.GetSubString(kvpStartIndex, keyEndIndex));
                String value(builder.GetSubString(keyEndIndex + 1, kvpEndIndex));
                query.Insert(key, value);
            }
            else {
                // only key
                String key(builder.GetSubString(kvpStartIndex, kvpEndIndex));
                query.Insert(key, String());
            }
            kvpStartIndex = kvpEndIndex + 1;
        }
        while (EndOfString != kvpEndIndex);
        return query;
    }
    else {
        return Map<String, String>();
    }
}

} // namespace IO
} // namespace Oryol