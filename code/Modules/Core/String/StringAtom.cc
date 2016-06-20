//------------------------------------------------------------------------------
//  StringAtom.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <cstring>
#include "StringAtom.h"
#include "String.h"

namespace Oryol {
const char* StringAtom::emptyString = "";

//------------------------------------------------------------------------------
StringAtom::StringAtom(const String& rhs) {
    this->setupFromCString(rhs.AsCStr());
}

//------------------------------------------------------------------------------
void
StringAtom::operator=(const String& rhs) {
    this->Clear();
    this->setupFromCString(rhs.AsCStr());
}

//------------------------------------------------------------------------------
String
StringAtom::AsString() const {
    if (0 != this->data) {
        return String(this->data->str);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
void
StringAtom::copy(const StringAtom& rhs) {
    // check if rhs is from our thread, if yes the copy is quick,
    // if no we need to transfer it into this thread's string atom table
    if (rhs.data) {
        if (rhs.data->table == stringAtomTable::threadLocalPtr()) {
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
void
StringAtom::setupFromCString(const char* str) {

    if ((0 != str) && (str[0] != 0)) {
        // get my thread-local string atom table
        stringAtomTable* table = stringAtomTable::threadLocalPtr();
        
        // get hash of string
        int32_t hash = stringAtomTable::HashForString(str);
        
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
bool
StringAtom::operator==(const StringAtom& rhs) const {

    if (rhs.data == this->data) {
        // definitely identical
        return true;
    }
    else if (rhs.data && this->data) {
        // NOTE: we can compare string atoms from different threads for
        // equality or inequality, but not for less/greater
        if (rhs.data->table == this->data->table) {
            // same thread, and not identical
            return false;
        }
        else if (rhs.data->hash != this->data->hash) {
            // different threads and hashes differ, definitely different
            return false;
        }
        else {
            // different threads, same hash, must do string compare
            return 0 == std::strcmp(this->data->str, rhs.data->str);
        }
        return false;
    }
    else {
        // one has data, the other not
        return false;
    }
}

//------------------------------------------------------------------------------
bool
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

} // namespace Oryol