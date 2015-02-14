//------------------------------------------------------------------------------
//  resourceMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "Gfx/base/textureLoaderBase.h"
#include "resourceMgr.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
resourceMgr::resourceMgr() :
isValid(false),
runLoopId(RunLoop::InvalidId),
renderer(nullptr),
displayMgr(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
resourceMgr::~resourceMgr() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
resourceMgr::AttachLoader(const Ptr<loaderBase>& loader) {
    switch (loader->resourceType()) {
        case ResourceType::Texture:
            this->textureFactory.AttachLoader(loader.dynamicCast<textureLoaderBase>());
            break;
        default:
            o_error("resourceMgr::AttachLoader(): invalid loader resource type!\n");
            break;
    }
}

//------------------------------------------------------------------------------
void
resourceMgr::Setup(const GfxSetup& setup, class renderer* rendr, class displayMgr* dspMgr) {
    o_assert(!this->isValid);
    o_assert(nullptr != rendr);
    o_assert(nullptr != dspMgr);
    
    this->isValid = true;
    this->renderer = rendr;
    this->displayMgr = dspMgr;

    this->meshFactory.Setup(this->renderer, &this->meshPool);
    this->meshPool.Setup(&this->meshFactory, setup.PoolSize(ResourceType::Mesh), setup.Throttling(ResourceType::Mesh), 'MESH');
    this->shaderFactory.Setup();
    this->shaderPool.Setup(&this->shaderFactory, setup.PoolSize(ResourceType::Shader), 0, 'SHDR');
    this->programBundleFactory.Setup(this->renderer, &this->shaderPool, &this->shaderFactory);
    this->programBundlePool.Setup(&this->programBundleFactory, setup.PoolSize(ResourceType::ProgramBundle), 0, 'PRGB');
    this->textureFactory.Setup(this->renderer, this->displayMgr, &this->texturePool);
    this->texturePool.Setup(&this->textureFactory, setup.PoolSize(ResourceType::Texture), setup.Throttling(ResourceType::Texture), 'TXTR');
    this->drawStateFactory.Setup(&this->meshPool, &this->programBundlePool);
    this->drawStatePool.Setup(&this->drawStateFactory, setup.PoolSize(ResourceType::DrawState), 0, 'DRWS');
    
    // attach loaders
    for (const auto& loaderCreator : setup.Loaders) {
        Ptr<loaderBase> loader = loaderCreator();
        this->AttachLoader(loader);
    }
    
    // add out update method to the runloop
    this->runLoopId = Core::PreRunLoop()->Add([this] {
        this->Update();
    });
}

//------------------------------------------------------------------------------
void
resourceMgr::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    Core::PreRunLoop()->Remove(this->runLoopId);
    this->drawStatePool.Discard();
    this->drawStateFactory.Discard();
    this->texturePool.Discard();
    this->textureFactory.Discard();
    this->programBundlePool.Discard();
    this->programBundleFactory.Discard();
    this->shaderPool.Discard();
    this->shaderFactory.Discard();
    this->meshPool.Discard();
    this->meshFactory.Discard();
    this->renderer = nullptr;
}

//------------------------------------------------------------------------------
bool
resourceMgr::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
resourceMgr::Update() {
    o_assert(this->isValid);
    
    // only call Update on pools which support asynchronous resource loading
    this->meshPool.Update();
    this->texturePool.Update();
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const MeshSetup& setup) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!setup.ShouldSetupFromFileAsync());
    Id resId = this->meshPool.AllocId();
    this->meshPool.Assign(resId, setup);
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const MeshSetup& setup, const Ptr<Stream>& data) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!setup.ShouldSetupFromFileAsync());
    Id resId = this->meshPool.AllocId();
    this->meshPool.Assign(resId, setup, data);
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const TextureSetup& setup) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!setup.ShouldSetupFromFileAsync());
    Id resId = this->texturePool.AllocId();
    this->texturePool.Assign(resId, setup);
    return resId;
    }
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const TextureSetup& setup, const Ptr<Stream>& data) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!setup.ShouldSetupFromFileAsync());
    Id resId = this->texturePool.AllocId();
    this->texturePool.Assign(resId, setup, data);
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const ShaderSetup& setup) {
    o_assert_dbg(this->isValid);
    Id resId = this->shaderPool.AllocId();
    this->shaderPool.Assign(resId, setup);
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const ProgramBundleSetup& setup) {
    o_assert_dbg(this->isValid);
    Id resId = this->programBundlePool.AllocId();
    this->programBundlePool.Assign(resId, setup);
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const DrawStateSetup& setup) {
    o_assert_dbg(this->isValid);
    Id resId = this->drawStatePool.AllocId();
    this->drawStatePool.Assign(resId, setup);
    return resId;
}
    
//------------------------------------------------------------------------------
void
resourceMgr::DiscardResource(const Id& resId) {
    o_assert_dbg(this->isValid);
    switch (resId.Type()) {
        case ResourceType::Texture:
            this->texturePool.Unassign(resId);
            break;
        case ResourceType::Mesh:
            this->meshPool.Unassign(resId);
            break;
        case ResourceType::Shader:
            this->shaderPool.Unassign(resId);
            break;
        case ResourceType::ProgramBundle:
            this->programBundlePool.Unassign(resId);
            break;
        case ResourceType::ConstantBlock:
            o_assert2(false, "FIXME!!!\n");
            break;
        case ResourceType::DrawState:
            this->drawStatePool.Unassign(resId);
            break;
        default:
            o_assert(false);
            break;
    }
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
resourceMgr::QueryResourceState(const Id& resId) {
    o_assert_dbg(this->isValid);
    switch (resId.Type()) {
        case ResourceType::Texture:
            return this->texturePool.QueryState(resId);
        case ResourceType::Mesh:
            return this->meshPool.QueryState(resId);
        case ResourceType::Shader:
            return this->shaderPool.QueryState(resId);
        case ResourceType::ProgramBundle:
            return this->programBundlePool.QueryState(resId);
        case ResourceType::ConstantBlock:
            o_assert2(false, "FIXME!!!\n");
            break;
        case ResourceType::DrawState:
            return this->drawStatePool.QueryState(resId);
        default:
            o_assert(false);
            break;
    }
    return ResourceState::InvalidState;
}

} // namespace _priv
} // namespace Oryol