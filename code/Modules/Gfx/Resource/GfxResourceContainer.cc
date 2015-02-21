//------------------------------------------------------------------------------
//  GfxResourceContainer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "GfxResourceContainer.h"
#include "Core/Core.h"

namespace Oryol {

using namespace _priv;

//------------------------------------------------------------------------------
GfxResourceContainer::GfxResourceContainer() :
runLoopId(RunLoop::InvalidId),
renderer(nullptr),
displayMgr(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
void
GfxResourceContainer::setup(const GfxSetup& setup, class renderer* rendr, class displayMgr* dspMgr) {
    o_assert(!this->isValid());
    o_assert(nullptr != rendr);
    o_assert(nullptr != dspMgr);
    
    this->renderer = rendr;
    this->displayMgr = dspMgr;
    
    this->meshFactory.Setup(this->renderer, &this->meshPool);
    this->meshPool.Setup(ResourceType::Mesh, setup.PoolSize(ResourceType::Mesh));
    this->shaderFactory.Setup();
    this->shaderPool.Setup(ResourceType::Shader, setup.PoolSize(ResourceType::Shader));
    this->programBundleFactory.Setup(this->renderer, &this->shaderPool, &this->shaderFactory);
    this->programBundlePool.Setup(ResourceType::ProgramBundle, setup.PoolSize(ResourceType::ProgramBundle));
    this->textureFactory.Setup(this->renderer, this->displayMgr, &this->texturePool);
    this->texturePool.Setup(ResourceType::Texture, setup.PoolSize(ResourceType::Texture));
    this->drawStateFactory.Setup(&this->meshPool, &this->programBundlePool);
    this->drawStatePool.Setup(ResourceType::DrawState, setup.PoolSize(ResourceType::DrawState));
    
    resourceContainerBase::setup(setup.ResourceLabelStackCapacity, setup.ResourceRegistryCapacity);
}

//------------------------------------------------------------------------------
void
GfxResourceContainer::discard() {
    o_assert_dbg(this->isValid());
    
    resourceContainerBase::discard();
    
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
    this->displayMgr = nullptr;
}

//------------------------------------------------------------------------------
void
GfxResourceContainer::update() {
    o_assert_dbg(this->isValid());
    
    // only call Update on pools which support asynchronous resource loading
    // FIXME: MOVE ASYNC LOADING OUT OF RESOURCE POOLS
}

//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const MeshSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(!setup.ShouldSetupFromFile());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId(this->peekLabel());
        mesh& res = this->meshPool.Assign(resId, setup);
        this->meshFactory.SetupResource(res);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const MeshSetup& setup, const Ptr<Stream>& data) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(!setup.ShouldSetupFromFile());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId(this->peekLabel());
        mesh& res = this->meshPool.Assign(resId, setup);
        this->meshFactory.SetupResource(res, data);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}

//------------------------------------------------------------------------------
/*
template<> Id
GfxResourceContainer::Load(const MeshSetup& setup, const Ptr<MeshLoader>& loader) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(setup.ShouldSetupFromFile());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId(this->peekLabel());
        mesh& res = this->meshPool.Assign(resId, setup);
        // FIXME FIXME FIXME: call loader
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
*/
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const TextureSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(!setup.ShouldSetupFromFile());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId(this->peekLabel());
        texture& res = this->texturePool.Assign(resId, setup);
        this->textureFactory.SetupResource(res);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const TextureSetup& setup, const Ptr<Stream>& data) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(!setup.ShouldSetupFromFile());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId(this->peekLabel());
        texture& res = this->texturePool.Assign(resId, setup);
        this->textureFactory.SetupResource(res);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const ShaderSetup& setup) {
    o_assert_dbg(this->isValid());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->shaderPool.AllocId(this->peekLabel());
        shader& res = this->shaderPool.Assign(resId, setup);
        this->shaderFactory.SetupResource(res);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const ProgramBundleSetup& setup) {
    o_assert_dbg(this->isValid());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->programBundlePool.AllocId(this->peekLabel());
        programBundle& res = this->programBundlePool.Assign(resId, setup);
        this->programBundleFactory.SetupResource(res);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const DrawStateSetup& setup) {
    o_assert_dbg(this->isValid());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->drawStatePool.AllocId(this->peekLabel());
        drawState& res = this->drawStatePool.Assign(resId, setup);
        this->drawStateFactory.SetupResource(res);
        this->registry.Add(setup.Locator, resId);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
void
GfxResourceContainer::Destroy(uint8 label) {
    o_assert_dbg(this->isValid());
    Array<Id> ids = this->registry.Remove(label);
    for (const Id& id : ids) {
        switch (id.Type()) {
            case ResourceType::Texture:
            {
                texture* tex = this->texturePool.Lookup(id);
                if (tex) {
                    this->textureFactory.DestroyResource(*tex);
                }
                this->texturePool.Unassign(id);
            }
            break;
                
            case ResourceType::Mesh:
            {
                mesh* msh = this->meshPool.Lookup(id);
                if (msh) {
                    this->meshFactory.DestroyResource(*msh);
                }
                this->meshPool.Unassign(id);
            }
            break;
                
            case ResourceType::Shader:
            {
                shader* shd = this->shaderPool.Lookup(id);
                if (shd) {
                    this->shaderFactory.DestroyResource(*shd);
                }
                this->shaderPool.Unassign(id);
            }
            break;
                
            case ResourceType::ProgramBundle:
            {
                programBundle* prog = this->programBundlePool.Lookup(id);
                if (prog) {
                    this->programBundleFactory.DestroyResource(*prog);
                }
                this->programBundlePool.Unassign(id);
            }
            break;
                
            case ResourceType::ConstantBlock:
                o_assert2(false, "FIXME!!!\n");
                break;
                
            case ResourceType::DrawState:
            {
                drawState* ds = this->drawStatePool.Lookup(id);
                if (ds) {
                    this->drawStateFactory.DestroyResource(*ds);
                }
                this->drawStatePool.Unassign(id);
            }
            break;
                
            default:
                o_assert(false);
                break;
        }
    }
}
    
//------------------------------------------------------------------------------
ResourceState::Code
GfxResourceContainer::QueryState(const Id& resId) const {
    o_assert_dbg(this->isValid());
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

} // namespace Oryol