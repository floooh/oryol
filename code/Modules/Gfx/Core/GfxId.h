#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::GfxId
    @brief RAII wrapper for Resource::Id
    
    Wraps a resource id and manages its use count for automatic life time
    management of graphics resources.
*/
#include "Resource/Id.h"

namespace Oryol {
    
class GfxId {
public:
    /// default constructor
    GfxId();
    /// copy constructor
    GfxId(const GfxId& rhs);
    /// move constructor
    GfxId(GfxId&& rhs);
    /// construct from resource Id
    GfxId(const class Id& rhs);
    /// destructor
    ~GfxId();
    
    /// copy-assign
    void operator=(const GfxId& rhs);
    /// move-assign
    void operator=(GfxId&& rhs);
    
    /// equality
    bool operator==(const GfxId& rhs) const;
    /// inequality
    bool operator!=(const GfxId& rhs) const;
    /// less-then
    bool operator<(const GfxId& rhs) const;
    
    /// return true if resource Id is valid
    bool IsValid() const;
    /// release the embedded resource Id
    void Release();
    /// get the embedded resource Id
    const class Id& Id() const;
    
private:
    class Id resId;
};

//------------------------------------------------------------------------------
inline
GfxId::GfxId() {
    // empty
}

//------------------------------------------------------------------------------
inline
GfxId::GfxId(GfxId&& rhs) {
    this->resId = rhs.resId;
    rhs.resId.Invalidate();
}

//------------------------------------------------------------------------------
inline bool
GfxId::operator==(const GfxId& rhs) const {
    return this->resId == rhs.resId;
}

//------------------------------------------------------------------------------
inline bool
GfxId::operator!=(const GfxId& rhs) const {
    return this->resId != rhs.resId;
}

//------------------------------------------------------------------------------
inline bool
GfxId::operator<(const GfxId& rhs) const {
    return this->resId < rhs.resId;
}

//------------------------------------------------------------------------------
inline bool
GfxId::IsValid() const {
    return this->resId.IsValid();
}

//------------------------------------------------------------------------------
inline const class Id&
GfxId::Id() const {
    return this->resId;
}
    
} // namespace Oryol