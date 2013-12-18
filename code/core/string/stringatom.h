#pragma once
//------------------------------------------------------------------------------
/**
    @class string::stringatom
    
    A unique string, relatively slow on creation, but fast for comparison.
    Stringatoms are stored in thread-local stringatomtables and comparison
    is fastest in the creator thread.
*/
#include "core/types.h"
#include "core/string/stringatom_table.h"

namespace oryol {
namespace string {

class stringatom {
public:
    /// default constructor
    stringatom();
    /// construct from std::string (slow)
    explicit stringatom(const std::string& str);
    /// construct from raw string (slow)
    explicit stringatom(const char* str);
    /// construct from raw string (slow)
    explicit stringatom(const uchar* str);
    /// copy constructor (fast if rhs was created in same thread)
    explicit stringatom(const stringatom& rhs);
    /// destructor
    ~stringatom();
    
    /// assignment
    void operator=(const stringatom& rhs);
    /// assign raw string (slow)
    void operator=(const char* rhs);
    /// assign raw string (slow)
    void operator=(const uchar* rhs);
    /// assign from string object (slow)
    void operator=(const std::string& rhs);
    
    /// equality operator (FAST)
    bool operator==(const stringatom& rhs) const;
    /// inequality operator (FAST)
    bool operator!=(const stringatom& rhs) const;
    /// equality operator with raw string (SLOW!)
    bool operator==(const char* rhs) const;
    /// inequality operator with raw string (SLOW!)
    bool operator!=(const char* rhs) const;
    /// equality operator with raw string (SLOW!)
    bool operator==(const uchar* rhs) const;
    /// inequality operator with raw string (SLOW!)
    bool operator!=(const uchar* rhs) const;
    /// equality operator with std::string (SLOW!)
    bool operator==(const std::string& rhs) const;
    /// inequality operator with std::string (SLOW!)
    bool operator!=(const std::string& rhs) const;
    
    /// clear content (becomes invalid)
    void clear();
    /// return true if valid (contains a non-empty string)
    bool isvalid() const;
    /// get contained c-string
    const char* as_cstr() const;
    /// get std::string (slow because string object must be constructed)
    std::string as_string() const;

private:
    /// copy content
    void copy(const stringatom& rhs);
    /// setup from C string
    void setup_from_c_string(const char* str);
    
    const stringatom_buffer::header* data;
    static const char* empty_string;
};

//------------------------------------------------------------------------------
inline void stringatom::clear() {
    data = 0;
}

//------------------------------------------------------------------------------
inline void stringatom::copy(const stringatom& rhs) {
    // check if rhs is from our thread, if yes the copy is quick,
    // if no we need to transfer it into this thread's string atom table
    if (rhs.data) {
        if (rhs.data->table == stringatom_table::instance()) {
            data = rhs.data;
        }
        else {
            // rhs is from another thread, need to transfer to this thread
            setup_from_c_string(rhs.data->str);
        }
    }
    else {
        // fallthrough: rhs is invalid
        data = 0;
    }
}

//------------------------------------------------------------------------------
inline void stringatom::setup_from_c_string(const char* str) {

    if ((0 != str) && (str[0] != 0)) {
        // get my thread-local string atom table
        stringatom_table* table = stringatom_table::instance();
        
        // get hash of string
        std::size_t hash = stringatom_table::hash_for_string(str);
        
        // check if string already exists in table
        data = table->find(hash, str);
        if (0 == data) {
            // string doesn't exist yet in table, add it
            data = table->add(hash, str);
        }
    }
    else {
        // source was a null-ptr or empty string
        data = 0;
    }
}

//------------------------------------------------------------------------------
inline stringatom::stringatom()
    : data(0) {
    // empty
}

//------------------------------------------------------------------------------
inline stringatom::stringatom(const char* rhs) {
    setup_from_c_string(rhs);
}

//------------------------------------------------------------------------------
inline stringatom::stringatom(const uchar* rhs) {
    setup_from_c_string((const char*) rhs);
}

//------------------------------------------------------------------------------
inline stringatom::stringatom(const std::string& rhs) {
    setup_from_c_string(rhs.c_str());
}

//------------------------------------------------------------------------------
inline stringatom::stringatom(const stringatom& rhs) {
    copy(rhs);
}

//------------------------------------------------------------------------------
inline stringatom::~stringatom() {
    clear();
}

//------------------------------------------------------------------------------
inline void stringatom::operator=(const stringatom& rhs) {
    clear();
    copy(rhs);
}

//------------------------------------------------------------------------------
inline void stringatom::operator=(const char* rhs) {
    clear();
    setup_from_c_string(rhs);
}

//------------------------------------------------------------------------------
inline void stringatom::operator=(const uchar* rhs) {
    clear();
    setup_from_c_string((const char*)rhs);
}

//------------------------------------------------------------------------------
inline void stringatom::operator=(const std::string& rhs) {
    clear();
    setup_from_c_string((const char*) rhs.c_str());
}

//------------------------------------------------------------------------------
inline bool stringatom::operator==(const stringatom& rhs) const {

    if (rhs.data == this->data) {
        // definitely identical
        return true;
    }
    else if (rhs.data && data) {
        // both string atoms have data, if they are not from the same thread
        // we need to do a string compare
        if (rhs.data->table == data->table) {
            // from same thread, but data is not identical
            return false;
        }
        else {
            // different threads, quickly check hash
            if (rhs.data->hash != data->hash) {
                // different hashes, strings are different
                return false;
            }
            else {
                // same hash, need to do a string compare
                return 0 == std::strcmp(rhs.data->str, data->str);
            }
        }
    }
    else {
        // one has data, the other not
        return false;
    }
}

//------------------------------------------------------------------------------
inline bool stringatom::operator!=(const stringatom& rhs) const {
    return !operator==(rhs);
}

//------------------------------------------------------------------------------
inline bool stringatom::operator==(const char* rhs) const {
    if ((0 != data) && (0 != rhs)) {
        return (0 == std::strcmp(data->str, rhs));
    }
    else if ((0 == data) && (0 == rhs)) {
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
inline bool stringatom::operator!=(const char* rhs) const {
    return !operator==(rhs);
}

//------------------------------------------------------------------------------
inline bool stringatom::operator==(const uchar* rhs) const {
    return operator==((const char*) rhs);
}

//------------------------------------------------------------------------------
inline bool stringatom::isvalid() const {
    return (0 != data);
}

//------------------------------------------------------------------------------
inline const char* stringatom::as_cstr() const {
    if (0 != data) {
        return data->str;
    }
    else {
        return empty_string;
    }
}

//------------------------------------------------------------------------------
inline std::string stringatom::as_string() const {
    if (0 != data) {
        return std::string(data->str);
    }
    else {
        return std::string();
    }
}
    
} // namespace string
} // namespace oryol