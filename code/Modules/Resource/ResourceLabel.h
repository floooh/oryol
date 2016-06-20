#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceLabel
    @ingroup Resource
    @brief a label for lifetime management of resources
*/
#include "Core/Types.h"

namespace Oryol {

class ResourceLabel {
public:
    static const uint32_t Invalid = 0xFFFFFFFF;
    static const uint32_t All     = 0xFFFFFFFE;
    static const uint32_t Default = 0xFFFFFFFD;
    
    /// default constructor
    ResourceLabel() : Value(Invalid) { };
    /// construct from value
    ResourceLabel(uint32_t val) : Value(val) { };
    /// copy constructor
    ResourceLabel(const ResourceLabel& rhs) {
        this->Value = rhs.Value;
    };
    /// copy
    void operator=(const ResourceLabel& rhs) {
        this->Value = rhs.Value;
    };
    /// assign raw value
    void operator=(uint32_t val) {
        this->Value = val;
    };
    /// equality
    bool operator==(const ResourceLabel& rhs) const {
        return this->Value == rhs.Value;
    };
    /// inequality
    bool operator!=(const ResourceLabel& rhs) const {
        return this->Value != rhs.Value;
    };
    /// check if label has a valid value
    bool IsValid() const {
        return this->Value != Invalid;
    };
    /// invalidate the label
    void Invalidate() {
        this->Value = Invalid;
    };
    /// label value
    uint32_t Value;
};

//------------------------------------------------------------------------------
inline bool
operator==(const ResourceLabel& lhs, const uint32_t rhs) {
    return lhs.Value == rhs;
}

//------------------------------------------------------------------------------
inline bool
operator==(const uint32_t lhs, const ResourceLabel& rhs) {
    return lhs == rhs.Value;
}

//------------------------------------------------------------------------------
inline bool
operator!=(const ResourceLabel& lhs, const uint32_t rhs) {
    return lhs.Value != rhs;
}

//------------------------------------------------------------------------------
inline bool
operator!=(const uint32_t lhs, const ResourceLabel& rhs) {
    return lhs != rhs.Value;
}

} // namespace Oryol