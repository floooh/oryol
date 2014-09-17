//------------------------------------------------------------------------------
//  GfxId.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "GfxId.h"
#include "Gfx/Gfx.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
GfxId::GfxId(const GfxId& rhs) {
    this->resId = rhs.resId;
    if (this->resId.IsValid()) {
        Gfx::useResource(this->resId);
    }
}

//------------------------------------------------------------------------------
GfxId::GfxId(const class Id& rhs) {
    this->resId = rhs;
    if (this->resId.IsValid()) {
        Gfx::useResource(this->resId);
    }
}

//------------------------------------------------------------------------------
GfxId::~GfxId() {
    if (this->resId.IsValid()) {
        Gfx::releaseResource(this->resId);
        this->resId.Invalidate();
    }
}

//------------------------------------------------------------------------------
void
GfxId::operator=(const GfxId& rhs) {
    if (this->resId.IsValid()) {
        Gfx::releaseResource(this->resId);
        this->resId.Invalidate();
    }
    this->resId = rhs.resId;
    if (this->resId.IsValid()) {
        Gfx::useResource(this->resId);
    }
}

//------------------------------------------------------------------------------
void
GfxId::operator=(GfxId&& rhs) {
    if (this->resId.IsValid()) {
        Gfx::releaseResource(this->resId);
        this->resId.Invalidate();
    }
    this->resId = rhs.resId;
    rhs.resId.Invalidate();
}
    
//------------------------------------------------------------------------------
void
GfxId::Release() {
    if (this->resId.IsValid()) {
        Gfx::releaseResource(this->resId);
        this->resId.Invalidate();
    }
}

} // namespace Oryol
