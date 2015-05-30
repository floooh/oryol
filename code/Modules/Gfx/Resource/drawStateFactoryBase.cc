//------------------------------------------------------------------------------
//  drawStateFactoryBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawStateFactoryBase.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/meshPool.h"
#include "Gfx/Resource/programBundlePool.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
drawStateFactoryBase::drawStateFactoryBase() :
renderer(nullptr),
meshPool(nullptr),
programBundlePool(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
drawStateFactoryBase::~drawStateFactoryBase() {
    o_assert_dbg(nullptr == this->renderer);
    o_assert_dbg(nullptr == this->meshPool);
    o_assert_dbg(nullptr == this->programBundlePool);
}

//------------------------------------------------------------------------------
void
drawStateFactoryBase::Setup(class renderer* rendr, class meshPool* mshPool, class programBundlePool* pbPool) {
    o_assert_dbg(nullptr != rendr);
    o_assert_dbg(nullptr != mshPool);
    o_assert_dbg(nullptr != pbPool);
    o_assert_dbg(nullptr == this->renderer);
    o_assert_dbg(nullptr == this->meshPool);
    o_assert_dbg(nullptr == this->programBundlePool);

    this->renderer = rendr;
    this->meshPool = mshPool;
    this->programBundlePool = pbPool;
}

//------------------------------------------------------------------------------
void
drawStateFactoryBase::Discard() {
    o_assert_dbg(nullptr != this->renderer);
    o_assert_dbg(nullptr != this->meshPool);
    o_assert_dbg(nullptr != this->programBundlePool);
    
    this->renderer = nullptr;
    this->meshPool = nullptr;
    this->programBundlePool = nullptr;
}

//------------------------------------------------------------------------------
ResourceState::Code
drawStateFactoryBase::SetupResource(drawState& ds) {
    ds.msh = ds.Setup.Mesh;
    ds.prog = this->programBundlePool->Lookup(ds.Setup.Program);
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
drawStateFactoryBase::DestroyResource(drawState& ds) {
    ds.Clear();
}

} // namespace _priv
} // namespace Oryol
