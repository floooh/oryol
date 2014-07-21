//------------------------------------------------------------------------------
//  drawStateFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "drawStateFactory.h"
#include "Render/Core/drawState.h"
#include "Render/Core/meshPool.h"
#include "Render/Core/programBundlePool.h"

namespace Oryol {
namespace Render {

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
    o_assert(ds.GetState() == Resource::State::Setup);
    const DrawStateSetup& setup = ds.GetSetup();
    ds.setMesh(this->meshPool->Lookup(setup.Mesh));
    ds.setProgramBundle(this->programBundlePool->Lookup(setup.Program));
    ds.setState(Resource::State::Valid);
}

//------------------------------------------------------------------------------
void
drawStateFactory::DestroyResource(drawState& ds) {
    o_assert(ds.GetState() == Resource::State::Valid);
    ds.clear();
    ds.setState(Resource::State::Setup);
}

} // namespace Render
} // namespace Oryol