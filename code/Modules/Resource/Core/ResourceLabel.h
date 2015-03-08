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
    static const uint32 Invalid = 0xFFFFFFFF;
    static const uint32 All     = 0xFFFFFFFE;
    static const uint32 Default = 0xFFFFFFFD;
    
    /// default constructor
    ResourceLabel() : Value(Invalid) { };
    /// construct from value
    ResourceLabel(uint32 val) : Value(val) { };
    /// copy constructor
    ResourceLabel(const ResourceLabel& rhs) {
        this->Value = rhs.Value;
    };
    /// copy
    void operator=(const ResourceLabel& rhs) {
        this->Value = rhs.Value;
    };
    /// assign raw value
    void operator=(uint32 val) {
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
    /// label value
    uint32 Value;
};

//------------------------------------------------------------------------------
inline bool
operator==(const ResourceLabel& lhs, const uint32 rhs) {
    return lhs.Value == rhs;
}

//------------------------------------------------------------------------------
inline bool
operator==(const uint32 lhs, const ResourceLabel& rhs) {
    return lhs == rhs.Value;
}

//------------------------------------------------------------------------------
inline bool
operator!=(const ResourceLabel& lhs, const uint32 rhs) {
    return lhs.Value != rhs;
}

//------------------------------------------------------------------------------
inline bool
operator!=(const uint32 lhs, const ResourceLabel& rhs) {
    return lhs != rhs.Value;
}

} // namespace Oryol