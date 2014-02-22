//------------------------------------------------------------------------------
//  ContentType.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ContentType.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
namespace IO {

using namespace Core;

//------------------------------------------------------------------------------
void
ContentType::clearIndices() {
    for (int i = 0; i < numIndices; i++) {
        this->indices[i] = InvalidIndex;
    }
}

//------------------------------------------------------------------------------
void
ContentType::copyIndices(const ContentType& rhs) {
    for (int i = 0; i < numIndices; i++) {
        this->indices[i] = rhs.indices[i];
    }
}

//------------------------------------------------------------------------------
ContentType::ContentType() :
valid(false) {
    this->clearIndices();
}

//------------------------------------------------------------------------------
ContentType::ContentType(const ContentType& rhs) :
content(rhs.content),
valid(rhs.valid) {
    this->copyIndices(rhs);
}

//------------------------------------------------------------------------------
ContentType::ContentType(ContentType&& rhs) {
    this->content = std::move(rhs.content);
    this->copyIndices(rhs);
    this->valid = rhs.valid;
    rhs.clearIndices();
    rhs.valid = false;
}

//------------------------------------------------------------------------------
ContentType::ContentType(const char* rhs) :
content(rhs),
valid(false) {
    this->crack();
}

//------------------------------------------------------------------------------
ContentType::ContentType(const StringAtom& rhs) :
content(rhs),
valid(false) {
    this->crack();
}

//------------------------------------------------------------------------------
ContentType::ContentType(const String& rhs) :
content(rhs),
valid(false) {
    this->crack();
}

//------------------------------------------------------------------------------
void
ContentType::operator=(const ContentType& rhs) {
    if (&rhs != this) {
        this->content = rhs.content;
        this->copyIndices(rhs);
        this->valid = rhs.valid;
    }
}

//------------------------------------------------------------------------------
void
ContentType::operator=(ContentType&& rhs) {
    if (&rhs != this) {
        this->content = std::move(rhs.content);
        this->copyIndices(rhs);
        this->valid = rhs.valid;
        rhs.clearIndices();
        rhs.valid = false;
    }
}

//------------------------------------------------------------------------------
void
ContentType::operator=(const char* rhs) {
    this->content = rhs;
    this->crack();
}

//------------------------------------------------------------------------------
void
ContentType::operator=(const StringAtom& rhs) {
    this->content = rhs;
    this->crack();
}

//------------------------------------------------------------------------------
void
ContentType::operator=(const String& rhs) {
    this->content = rhs;
    this->crack();
}

//------------------------------------------------------------------------------
bool
ContentType::operator==(const ContentType& rhs) const {
    return this->content == rhs.content;
}

//------------------------------------------------------------------------------
bool
ContentType::operator!=(const ContentType& rhs) const {
    return this->content != rhs.content;
}

//------------------------------------------------------------------------------
bool
ContentType::IsValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
bool
ContentType::Empty() const {
    return this->content.Empty();
}

//------------------------------------------------------------------------------
const StringAtom&
ContentType::Get() const {
    return this->content;
}

//------------------------------------------------------------------------------
const char*
ContentType::AsCStr() const {
    return this->content.AsCStr();
}

//------------------------------------------------------------------------------
bool
ContentType::HasType() const {
    return InvalidIndex != this->indices[typeStart];
}

//------------------------------------------------------------------------------
String
ContentType::Type() const {
    if (this->HasType()) {
        return String(this->content.AsCStr(), this->indices[typeStart], this->indices[typeEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
ContentType::HasSubType() const {
    return InvalidIndex != this->indices[subTypeStart];
}

//------------------------------------------------------------------------------
String
ContentType::SubType() const {
    if (this->HasSubType()) {
        return String(this->content.AsCStr(), this->indices[subTypeStart], this->indices[subTypeEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
String
ContentType::TypeAndSubType() const {
    if (this->HasType()) {
        if (this->HasSubType()) {
            return String(this->content.AsCStr(), this->indices[typeStart], this->indices[subTypeEnd]);
        }
        else {
            return String(this->content.AsCStr(), this->indices[typeStart], this->indices[typeEnd]);
        }
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
ContentType::HasParams() const {
    return InvalidIndex != this->indices[paramStart];
}

//------------------------------------------------------------------------------
Map<String,String>
ContentType::Params() const {
    if (this->HasParams()) {
        Map<String,String> query;
        StringBuilder builder;
        builder.Set(this->content.AsCStr(), this->indices[paramStart], this->indices[paramEnd]);
        int32 kvpStartIndex = 0;
        int32 kvpEndIndex = 0;
        do {
            // params are of the form "key=value; key=value; ..."
            
            // skip spaces at start
            kvpStartIndex = builder.FindFirstNotOf(kvpStartIndex, EndOfString, " ");
            // get end of key/value pair
            kvpEndIndex = builder.FindFirstOf(kvpStartIndex, EndOfString, ";");
            // find end of key
            int32 keyEndIndex = builder.FindFirstOf(kvpStartIndex, kvpEndIndex, "=");
            if (EndOfString != keyEndIndex) {
                String key(builder.GetSubString(kvpStartIndex, keyEndIndex));
                String value(builder.GetSubString(keyEndIndex + 1, kvpEndIndex));
                query.Insert(key, value);
            }
            else {
                // hmmm, a key without a value, this is now allowed, skip it
                Log::Warn("ContentType: param without value in '%s'; ignored!\n", this->content.AsCStr());
            }
            kvpStartIndex = kvpEndIndex + 1;
        }
        while (EndOfString != kvpEndIndex);
        return query;
    }
    else {
        return Map<String,String>();
    }
}

//------------------------------------------------------------------------------
void
ContentType::crack() {
    this->clearIndices();
    this->valid = false;
    
    if (this->content.IsValid()) {
        StringBuilder builder;
        builder.Set(this->content);
        
        // extract type
        this->indices[typeStart] = 0;
        this->indices[typeEnd] = builder.FindFirstOf(0, EndOfString, "/");
        if (EndOfString == this->indices[typeEnd]) {
            Log::Warn("ContentType::crack(): '%s' is not a valid content type!\n", this->content.AsCStr());
            this->clearIndices();
            return;
        }
        
        // extract subtype
        this->indices[subTypeStart] = this->indices[typeEnd] + 1;
        this->indices[subTypeEnd] = builder.FindFirstOf(this->indices[subTypeStart], EndOfString, ";");
        
        // are any params present?
        if (EndOfString != this->indices[subTypeEnd]) {
            this->indices[paramStart] = builder.FindFirstNotOf(this->indices[subTypeEnd] + 1, EndOfString, " ");
            this->indices[paramEnd] = EndOfString;
        }
        this->valid = true;
    }
}

} // namespace IO
} // namespace Oryol