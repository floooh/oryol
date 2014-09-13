#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::KeyValuePair
    @ingroup Core
    @brief key-value-pair for mapping containers
    
    Key-value-pair template class. Similar to pair<>, but members are not
    called first and second, but Key() and Value().
    
    @see Map
*/
#include "Core/Config.h"

namespace Oryol {

template<class KEY, class VALUE> class KeyValuePair {
public:
    /// default constructor
    KeyValuePair();
    /// copy-construct from key and value
    KeyValuePair(const KEY& k, const VALUE& v);
    /// move-construct from key and value
    KeyValuePair(KEY&& k, VALUE&& v);
    /// copy-constructor
    KeyValuePair(const KeyValuePair& rhs);
    /// move-constructor
    KeyValuePair(KeyValuePair&& rhs);
    
    /// copy-assignment
    void operator=(const KeyValuePair& rhs);
    /// move-assignment
    void operator=(KeyValuePair&& rhs);
    
    /// test equality
    bool operator==(const KeyValuePair& rhs) const;
    /// test inequality
    bool operator!=(const KeyValuePair& rhs) const;
    /// test less-then
    bool operator<(const KeyValuePair& rhs) const;
    /// test greater-then
    bool operator>(const KeyValuePair& rhs) const;
    /// test less-or-equal
    bool operator<=(const KeyValuePair& rhs) const;
    /// test greater-or-equal
    bool operator>=(const KeyValuePair& rhs) const;
    
    /// get key
    const KEY& Key() const;
    /// read/write access to value
    VALUE& Value();
    /// read-only access to value
    const VALUE& Value() const;

    KEY key;
    VALUE value;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
KeyValuePair<KEY, VALUE>::KeyValuePair() {
    // empty
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
KeyValuePair<KEY, VALUE>::KeyValuePair(const KEY& k, const VALUE& v) :
key(k),
value(v) {
    // empty
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
KeyValuePair<KEY, VALUE>::KeyValuePair(KEY&& k, VALUE&& v) :
key(std::move(k)),
value(std::move(v)) {
    // empty
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
KeyValuePair<KEY, VALUE>::KeyValuePair(const KeyValuePair& rhs) :
key(rhs.key),
value(rhs.value) {
    // empty
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE>
KeyValuePair<KEY, VALUE>::KeyValuePair(KeyValuePair&& rhs) :
key(std::move(rhs.key)),
value(std::move(rhs.value)) {
    // empty
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
KeyValuePair<KEY, VALUE>::operator=(const KeyValuePair& rhs) {
    if (&rhs != this) {
        this->key = rhs.key;
        this->value = rhs.value;
    }
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> void
KeyValuePair<KEY, VALUE>::operator=(KeyValuePair&& rhs) {
    if (&rhs != this) {
        this->key = std::move(rhs.key);
        this->value = std::move(rhs.value);
    }
}
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
KeyValuePair<KEY, VALUE>::operator==(const KeyValuePair& rhs) const {
    return this->key == rhs.key;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
KeyValuePair<KEY, VALUE>::operator!=(const KeyValuePair& rhs) const {
    return this->key != rhs.key;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
KeyValuePair<KEY, VALUE>::operator<(const KeyValuePair& rhs) const {
    return this->key < rhs.key;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
KeyValuePair<KEY, VALUE>::operator>(const KeyValuePair& rhs) const {
    return this->key > rhs.key;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
KeyValuePair<KEY, VALUE>::operator<=(const KeyValuePair& rhs) const {
    return this->key <= rhs.key;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> bool
KeyValuePair<KEY, VALUE>::operator>=(const KeyValuePair& rhs) const {
    return this->key >= rhs.key;
};
    
//------------------------------------------------------------------------------
template<class KEY, class VALUE> const KEY&
KeyValuePair<KEY, VALUE>::Key() const {
    return this->key;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> VALUE&
KeyValuePair<KEY, VALUE>::Value() {
    return this->value;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> const VALUE&
KeyValuePair<KEY, VALUE>::Value() const {
    return this->value;
}

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator==(const KeyValuePair<KEY, VALUE>& kvp, const KEY& key) {
    return kvp.key == key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator!=(const KeyValuePair<KEY, VALUE>& kvp, const KEY& key) {
    return kvp.key != key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator>(const KeyValuePair<KEY, VALUE>& kvp, const KEY& key) {
    return kvp.key > key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator<(const KeyValuePair<KEY, VALUE>& kvp, const KEY& key) {
    return kvp.key < key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator>=(const KeyValuePair<KEY, VALUE>& kvp, const KEY& key) {
    return kvp.key >= key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator<=(const KeyValuePair<KEY, VALUE>& kvp, const KEY& key) {
    return kvp.key >= key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator==(const KEY& key, const KeyValuePair<KEY, VALUE>& kvp) {
    return key == kvp.key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator!=(const KEY& key, const KeyValuePair<KEY, VALUE>& kvp) {
    return key != kvp.key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator>(const KEY& key, const KeyValuePair<KEY, VALUE>& kvp) {
    return key > kvp.key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator<(const KEY& key, const KeyValuePair<KEY, VALUE>& kvp) {
    return key < kvp.key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator>=(const KEY& key, const KeyValuePair<KEY, VALUE>& kvp) {
    return key >= kvp.key;
};

//------------------------------------------------------------------------------
template<class KEY, class VALUE> inline bool operator<=(const KEY& key, const KeyValuePair<KEY, VALUE>& kvp) {
    return key <= kvp.key;
};

} // namespace Oryol