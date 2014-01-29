#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::StringAtom
    @brief immutable, unique strings for fast comparison
    
    A unique string, relatively slow on creation, but fast for comparison.
    String atoms are stored in thread-local stringAtomTables and comparison
    is fastest in the creator thread.
    
    @see String
*/
#include "Core/Types.h"
#include "Core/String/stringAtomTable.h"

namespace Oryol {
namespace Core {

class String;

class StringAtom {
public:
    /// default constructor
    StringAtom();
    /// construct from String
    explicit StringAtom(const String& str);
    /// construct from raw string (slow)
    explicit StringAtom(const char* str);
    /// construct from raw string (slow)
    explicit StringAtom(const uchar* str);
    /// copy-constructor (fast if rhs was created in same thread)
    StringAtom(const StringAtom& rhs);
    /// move-constructor
    StringAtom(StringAtom&& rhs);
    /// destructor
    ~StringAtom();
    
    /// assignment
    void operator=(const StringAtom& rhs);
    /// assign raw string (slow)
    void operator=(const char* rhs);
    /// assign raw string (slow)
    void operator=(const uchar* rhs);
    /// assign from String object (slow)
    void operator=(const String& rhs);
    
    /// equality operator (FAST)
    bool operator==(const StringAtom& rhs) const;
    /// inequality operator (FAST)
    bool operator!=(const StringAtom& rhs) const;
    /// equality operator with raw string (SLOW!)
    bool operator==(const char* rhs) const;
    /// inequality operator with raw string (SLOW!)
    bool operator!=(const char* rhs) const;
    /// equality operator with raw string (SLOW!)
    bool operator==(const uchar* rhs) const;
    /// inequality operator with raw string (SLOW!)
    bool operator!=(const uchar* rhs) const;
    /// equality operator with String (SLOW!)
    bool operator==(const String& rhs) const;
    /// inequality operator with String (SLOW!)
    bool operator!=(const String& rhs) const;
    
    /// clear content (becomes invalid)
    void Clear();
    /// return true if valid (contains a non-empty string)
    bool IsValid() const;
    /// get length
    int32 Length() const;
    /// get contained c-string
    const char* AsCStr() const;
    /// get String (slow because string object must be constructed)
    String AsString() const;

private:
    /// copy content
    void copy(const StringAtom& rhs);
    /// setup from C string
    void setupFromCString(const char* str);
    
    const stringAtomBuffer::Header* data;
    static const char* emptyString;
};

//------------------------------------------------------------------------------
inline void
StringAtom::Clear() {
    this->data = nullptr;
}

//------------------------------------------------------------------------------
inline void
StringAtom::copy(const StringAtom& rhs) {
    // check if rhs is from our thread, if yes the copy is quick,
    // if no we need to transfer it into this thread's string atom table
    if (rhs.data) {
        if (rhs.data->table == stringAtomTable::Instance()) {
            this->data = rhs.data;
        }
        else {
            // rhs is from another thread, need to transfer to this thread
            this->setupFromCString(rhs.data->str);
        }
    }
    else {
        // fallthrough: rhs is invalid
        this->data = nullptr;
    }
}

//------------------------------------------------------------------------------
inline void
StringAtom::setupFromCString(const char* str) {

    if ((0 != str) && (str[0] != 0)) {
        // get my thread-local string atom table
        stringAtomTable* table = stringAtomTable::Instance();
        
        // get hash of string
        std::size_t hash = stringAtomTable::HashForString(str);
        
        // check if string already exists in table
        this->data = table->Find(hash, str);
        if (0 == this->data) {
            // string doesn't exist yet in table, add it
            this->data = table->Add(hash, str);
        }
    }
    else {
        // source was a null-ptr or empty string
        this->data = nullptr;
    }
}

//------------------------------------------------------------------------------
inline
StringAtom::StringAtom()
    : data(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
inline
StringAtom::StringAtom(const char* rhs) {
    this->setupFromCString(rhs);
}

//------------------------------------------------------------------------------
inline
StringAtom::StringAtom(const uchar* rhs) {
    this->setupFromCString((const char*) rhs);
}

//------------------------------------------------------------------------------
inline
StringAtom::StringAtom(const StringAtom& rhs) {
    this->copy(rhs);
}

//------------------------------------------------------------------------------
inline
StringAtom::StringAtom(StringAtom&& rhs) {
    this->copy(rhs);
    rhs.data = nullptr;
}

//------------------------------------------------------------------------------
inline
StringAtom::~StringAtom() {
    this->Clear();
}

//------------------------------------------------------------------------------
inline void
StringAtom::operator=(const StringAtom& rhs) {
    if (&rhs != this) {
        this->Clear();
        this->copy(rhs);
    }
}

//------------------------------------------------------------------------------
inline void
StringAtom::operator=(const char* rhs) {
    this->Clear();
    this->setupFromCString(rhs);
}

//------------------------------------------------------------------------------
inline void
StringAtom::operator=(const uchar* rhs) {
    this->Clear();
    this->setupFromCString((const char*)rhs);
}

//------------------------------------------------------------------------------
inline bool
StringAtom::operator==(const StringAtom& rhs) const {

    if (rhs.data == this->data) {
        // definitely identical
        return true;
    }
    else if (rhs.data && this->data) {
        // both string atoms have data, if they are not from the same thread
        // we need to do a string compare
        if (rhs.data->table == this->data->table) {
            // from same thread, but data is not identical
            return false;
        }
        else {
            // different threads, quickly check hash
            if (rhs.data->hash != this->data->hash) {
                // different hashes, strings are different
                return false;
            }
            else {
                // same hash, need to do a string compare
                return 0 == std::strcmp(rhs.data->str, this->data->str);
            }
        }
    }
    else {
        // one has data, the other not
        return false;
    }
}

//------------------------------------------------------------------------------
inline bool
StringAtom::operator!=(const StringAtom& rhs) const {
    return !operator==(rhs);
}

//------------------------------------------------------------------------------
inline bool
StringAtom::operator==(const char* rhs) const {
    if ((0 != this->data) && (0 != rhs)) {
        return (0 == std::strcmp(this->data->str, rhs));
    }
    else if ((0 == this->data) && (0 == rhs)) {
        return true;
    }
    else if ((0 == this->data) && (0 != rhs)) {
        return (rhs[0] == 0);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
inline bool
StringAtom::operator!=(const char* rhs) const {
    return !operator==(rhs);
}

//------------------------------------------------------------------------------
inline bool
StringAtom::operator==(const uchar* rhs) const {
    return operator==((const char*) rhs);
}

//------------------------------------------------------------------------------
inline bool
StringAtom::IsValid() const {
    return (0 != this->data);
}

//------------------------------------------------------------------------------
inline int32
StringAtom::Length() const {
    if (nullptr != this->data) {
        return this->data->length;
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
inline const char*
StringAtom::AsCStr() const {
    if (nullptr != this->data) {
        return this->data->str;
    }
    else {
        return emptyString;
    }
}

} // namespace Core
} // namespace Oryol
