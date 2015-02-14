#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::AssetId
    @ingroup Assets
    @brief RAII wrapper for Resource::Id
    
    Wraps a resource id and manages its use count for automatic life time
    management of assets through the asset registry.
*/
#include "Resource/Id.h"

namespace Oryol {
    
class AssetId {
public:
    /// default constructor
    AssetId();
    /// copy constructor
    AssetId(const AssetId& rhs);
    /// move constructor
    AssetId(AssetId&& rhs);
    /// construct from resource Id
    AssetId(const class Id& rhs);
    /// destructor
    ~AssetId();
    
    /// copy-assign
    void operator=(const AssetId& rhs);
    /// move-assign
    void operator=(AssetId&& rhs);
    
    /// equality
    bool operator==(const AssetId& rhs) const;
    /// inequality
    bool operator!=(const AssetId& rhs) const;
    /// less-then
    bool operator<(const AssetId& rhs) const;
    /// cast to resource id
    operator Id() const;
    
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
AssetId::AssetId() {
    // empty
}

//------------------------------------------------------------------------------
inline
AssetId::AssetId(AssetId&& rhs) {
    this->resId = rhs.resId;
    rhs.resId.Invalidate();
}

//------------------------------------------------------------------------------
inline bool
AssetId::operator==(const AssetId& rhs) const {
    return this->resId == rhs.resId;
}

//------------------------------------------------------------------------------
inline bool
AssetId::operator!=(const AssetId& rhs) const {
    return this->resId != rhs.resId;
}

//------------------------------------------------------------------------------
inline bool
AssetId::operator<(const AssetId& rhs) const {
    return this->resId < rhs.resId;
}

//------------------------------------------------------------------------------
inline
AssetId::operator class Id() const {
    return this->resId;
}

//------------------------------------------------------------------------------
inline bool
AssetId::IsValid() const {
    return this->resId.IsValid();
}

//------------------------------------------------------------------------------
inline const class Id&
AssetId::Id() const {
    return this->resId;
}
    
} // namespace Oryol
