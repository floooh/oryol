//------------------------------------------------------------------------------
//  drawStateFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawStateFactory.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/meshPool.h"
#include "Gfx/Resource/programBundlePool.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
drawStateFactory::drawStateFactory() :
meshPool(nullptr),
programBundlePool(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
drawStateFactory::~drawStateFactory() {
    o_assert(nullptr == this->meshPool);
    o_assert(nullptr == this->programBundlePool);
}

//------------------------------------------------------------------------------
void
drawStateFactory::Setup(class meshPool* mshPool, class programBundlePool* pbPool) {
    o_assert(nullptr != mshPool);
    o_assert(nullptr != pbPool);
    o_assert(nullptr == this->meshPool);
    o_assert(nullptr == this->programBundlePool);

    this->meshPool = mshPool;
    this->programBundlePool = pbPool;
}

//------------------------------------------------------------------------------
void
drawStateFactory::Discard() {
    o_assert(nullptr != this->meshPool);
    o_assert(nullptr != this->programBundlePool);
    
    this->meshPool = nullptr;
    this->programBundlePool = nullptr;
}

//------------------------------------------------------------------------------
void
drawStateFactory::SetupResource(drawState& ds) {
    o_assert(ResourceState::Setup == ds.State);
    ds.mesh = this->meshPool->Lookup(ds.Setup.Mesh);
    ds.programBundle = this->programBundlePool->Lookup(ds.Setup.Program);
    ds.State = ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
drawStateFactory::DestroyResource(drawState& ds) {
    o_assert(ResourceState::Valid == ds.State);
    ds.Clear();
    ds.State = ResourceState::Setup;
}

} // namespace _priv
} // namespace Oryol
