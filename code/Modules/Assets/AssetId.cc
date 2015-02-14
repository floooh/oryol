//------------------------------------------------------------------------------
//  AssetId.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "AssetId.h"
#include "Assets/Assets.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
AssetId::AssetId(const AssetId& rhs) {
    this->resId = rhs.resId;
    if (this->resId.IsValid()) {
        Assets::useAsset(this->resId);
    }
}

//------------------------------------------------------------------------------
AssetId::AssetId(const class Id& rhs) {
    this->resId = rhs;
    if (this->resId.IsValid()) {
        Assets::useAsset(this->resId);
    }
}

//------------------------------------------------------------------------------
AssetId::~AssetId() {
    if (this->resId.IsValid()) {
        Assets::releaseAsset(this->resId);
        this->resId.Invalidate();
    }
}

//------------------------------------------------------------------------------
void
AssetId::operator=(const AssetId& rhs) {
    if (this->resId.IsValid()) {
        Assets::releaseAsset(this->resId);
        this->resId.Invalidate();
    }
    this->resId = rhs.resId;
    if (this->resId.IsValid()) {
        Assets::useAsset(this->resId);
    }
}

//------------------------------------------------------------------------------
void
AssetId::operator=(AssetId&& rhs) {
    if (this->resId.IsValid()) {
        Assets::releaseAsset(this->resId);
        this->resId.Invalidate();
    }
    this->resId = rhs.resId;
    rhs.resId.Invalidate();
}
    
//------------------------------------------------------------------------------
void
AssetId::Release() {
    if (this->resId.IsValid()) {
        Assets::releaseAsset(this->resId);
        this->resId.Invalidate();
    }
}

} // namespace Oryol
