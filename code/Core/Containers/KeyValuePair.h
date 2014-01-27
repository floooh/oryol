#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::KeyValuePair
    @brief key-value-pair for mapping containers
    
    Key-value-pair template class. Similar to pair<>, but members are not
    called first and second, but Key() and Value().
    
    @see Map
*/
#include "Core/Config.h"

namespace Oryol {
namespace Core {

template<class KEY, class VALUE> class KeyValuePair {
public:
    /// default constructor
    KeyValuePair() {
    };
    /// copy-construct from key and value
    KeyValuePair(const KEY& k, const VALUE& v) : key(k), value(v) {
    };
    /// move-construct from key and value
    KeyValuePair(KEY&& k, VALUE&& v) : key(std::move(k)), value(std::move(v)) {
    };
    /// copy-constructor
    KeyValuePair(const KeyValuePair& rhs) : key(rhs.key), value(rhs.value) {
    };
    /// move-constructor
    KeyValuePair(KeyValuePair&& rhs) : key(std::move(rhs.key)), value(std::move(rhs.value)) {
    };
    
    /// copy-assignment
    void operator=(const KeyValuePair& rhs) {
        if (&rhs != this) {
            this->key = rhs.key;
            this->value = rhs.value;
        }
    };
    /// move-assignment
    void operator=(KeyValuePair&& rhs) {
        if (&rhs != this) {
            this->key = std::move(rhs.key);
            this->value = std::move(rhs.value);
        }
    };
    
    /// test equality
    bool operator==(const KeyValuePair& rhs) const {
        return this->key == rhs.key;
    };
    /// test inequality
    bool operator!=(const KeyValuePair& rhs) const {
        return this->key != rhs.key;
    };
    /// test less-then
    bool operator<(const KeyValuePair& rhs) const {
        return this->key < rhs.key;
    };
    /// test greater-then
    bool operator>(const KeyValuePair& rhs) const {
        return this->key > rhs.key;
    };
    /// test less-or-equal
    bool operator<=(const KeyValuePair& rhs) const {
        return this->key <= rhs.key;
    };
    /// test greater-or-equal
    bool operator>=(const KeyValuePair& rhs) const {
        return this->key >= rhs.key;
    };
    
    /// get key
    const KEY& Key() const {
        return this->key;
    };
    /// read/write access to value
    VALUE& Value() {
        return this->value;
    };
    /// read-only access to value
    const VALUE& Value() const {
        return this->value;
    };

    KEY key;
    VALUE value;
};

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

} // namespace Core
} // namespace Oryol