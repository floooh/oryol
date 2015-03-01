//------------------------------------------------------------------------------
//  GfxResourceContainer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "IO/IO.h"
#include "GfxResourceContainer.h"
#include "Gfx/Core/displayMgr.h"

namespace Oryol {

using namespace _priv;

//------------------------------------------------------------------------------
GfxResourceContainer::GfxResourceContainer() :
renderer(nullptr),
displayMgr(nullptr),
runLoopId(RunLoop::InvalidId) {
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
    this->meshPool.Setup(GfxResourceType::Mesh, setup.PoolSize(GfxResourceType::Mesh));
    this->shaderFactory.Setup();
    this->shaderPool.Setup(GfxResourceType::Shader, setup.PoolSize(GfxResourceType::Shader));
    this->programBundleFactory.Setup(this->renderer, &this->shaderPool, &this->shaderFactory);
    this->programBundlePool.Setup(GfxResourceType::ProgramBundle, setup.PoolSize(GfxResourceType::ProgramBundle));
    this->textureFactory.Setup(this->renderer, this->displayMgr, &this->texturePool);
    this->texturePool.Setup(GfxResourceType::Texture, setup.PoolSize(GfxResourceType::Texture));
    this->drawStateFactory.Setup(&this->meshPool, &this->programBundlePool);
    this->drawStatePool.Setup(GfxResourceType::DrawState, setup.PoolSize(GfxResourceType::DrawState));
    
    this->runLoopId = Core::PostRunLoop()->Add([this]() {
        this->UpdatePending();
    });
    
    resourceContainerBase::setup(setup.ResourceLabelStackCapacity, setup.ResourceRegistryCapacity);
}

//------------------------------------------------------------------------------
void
GfxResourceContainer::discard() {
    o_assert_dbg(this->isValid());
    
    Core::PostRunLoop()->Remove(this->runLoopId);
    
    resourceContainerBase::discard();

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
template<> Id
GfxResourceContainer::Create(const MeshSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        mesh& res = this->meshPool.Assign(resId, setup, ResourceState::Setup);
        ResourceState::Code newState = this->meshFactory.SetupResource(res) ? ResourceState::Valid : ResourceState::Failed;
        this->meshPool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const SetupAndStream<MeshSetup>& setupAndStream) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    const MeshSetup& setup = setupAndStream.Setup;
    const Ptr<Stream>& data = setupAndStream.Stream;
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        mesh& res = this->meshPool.Assign(resId, setup, ResourceState::Setup);
        ResourceState::Code newState = this->meshFactory.SetupResource(res, data) ? ResourceState::Valid : ResourceState::Failed;
        this->meshPool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const TextureSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        texture& res = this->texturePool.Assign(resId, setup, ResourceState::Setup);
        ResourceState::Code newState = this->textureFactory.SetupResource(res) ? ResourceState::Valid : ResourceState::Failed;
        this->texturePool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const SetupAndStream<TextureSetup>& setupAndStream) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());

    const TextureSetup& setup = setupAndStream.Setup;
    const Ptr<Stream>& data = setupAndStream.Stream;
    o_assert_dbg(!setup.ShouldSetupFromFile());
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        texture& res = this->texturePool.Assign(resId, setup, ResourceState::Setup);
        ResourceState::Code newState = this->textureFactory.SetupResource(res, data) ? ResourceState::Valid : ResourceState::Failed;
        this->texturePool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Load(const TextureSetup& setup, int32 ioLane, std::function<Ptr<TextureLoaderBase>()> loaderCreator) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    o_assert_dbg(setup.ShouldSetupFromFile());
    
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        this->texturePool.Assign(resId, setup, ResourceState::Pending);
        
        auto loader = loaderCreator();
        loader->Prepare(resId, setup);
        Ptr<IOProtocol::Request> ioReq = IOProtocol::Request::Create();
        ioReq->SetURL(setup.Locator.Location());
        ioReq->SetLane(ioLane);
        ioReq->SetLoader(loader);
        IO::Put(ioReq);

        return resId;
    }
}

//------------------------------------------------------------------------------
template<> void
GfxResourceContainer::initResourceFromThread(int32 threadIndex, const Id& resId, const SetupAndStream<TextureSetup>& setupAndStream) {
    // NOTE: this is running on a resource creation thread on target platforms
    // that support threading!
    o_assert(this->isValid());
    o_assert(this->texturePool.QueryState(resId) == ResourceState::Pending);
    
    // create resource and return ResourceStreamTarget
    texture& tex = this->texturePool.Assign(resId, setupAndStream.Setup, ResourceState::Pending);
    bool success = this->textureFactory.InitResourceThreaded(threadIndex, tex, setupAndStream.Stream);
    
    // tell the main thread that this resource has finished creation
    this->pendingCreateLock.LockWrite();
    this->pendingCreates.Add(resId, success ? ResourceState::Valid : ResourceState::Failed);
    this->pendingCreateLock.UnlockWrite();
}

//------------------------------------------------------------------------------
void
GfxResourceContainer::failResourceFromThread(int32 threadIndex, const Id& resId) {
    // NOTE: this is running on a resource creation thread on target platforms
    // that support threading!
    o_assert(this->isValid());
    o_assert(this->texturePool.QueryState(resId) == ResourceState::Pending);
    
    // tell the main thread that resource creation had failed
    this->pendingCreateLock.LockWrite();
    this->pendingCreates.Add(resId, ResourceState::Failed);
    this->pendingCreateLock.UnlockWrite();
}

//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const ShaderSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->shaderPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        shader& res = this->shaderPool.Assign(resId, setup, ResourceState::Setup);
        ResourceState::Code newState = this->shaderFactory.SetupResource(res) ? ResourceState::Valid : ResourceState::Failed;
        this->shaderPool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const ProgramBundleSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->programBundlePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        programBundle& res = this->programBundlePool.Assign(resId, setup, ResourceState::Setup);
        ResourceState::Code newState = this->programBundleFactory.SetupResource(res) ? ResourceState::Valid : ResourceState::Failed;
        this->programBundlePool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
GfxResourceContainer::Create(const DrawStateSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->drawStatePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        drawState& res = this->drawStatePool.Assign(resId, setup, ResourceState::Setup);
        ResourceState::Code newState = this->drawStateFactory.SetupResource(res) ? ResourceState::Valid : ResourceState::Failed;
        this->drawStatePool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
void
GfxResourceContainer::Destroy(ResourceLabel label) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    Array<Id> ids = this->registry.Remove(label);
    for (const Id& id : ids) {
        // NOTE: all resources are currently loading (can only be textures
        // and meshes) are added to the pendingDestroys array for deferred
        // destruction after loading has finished
        switch (id.Type()) {
            case GfxResourceType::Texture:
            {
                ResourceState::Code state = this->texturePool.QueryState(id);
                if (ResourceState::Pending == state) {
                    this->pendingDestroys.Add(id);
                }
                else {
                    if (ResourceState::Valid == state) {
                        texture* tex = this->texturePool.Lookup(id);
                        if (tex) {
                            this->textureFactory.DestroyResource(*tex);
                        }
                    }
                    this->texturePool.Unassign(id, ResourceState::Initial);
                }
            }
            break;
                
            case GfxResourceType::Mesh:
            {
                ResourceState::Code state = this->meshPool.QueryState(id);
                if (ResourceState::Pending == state) {
                    this->pendingDestroys.Add(id);
                }
                else {
                    if (ResourceState::Valid == state) {
                        mesh* msh = this->meshPool.Lookup(id);
                        if (msh) {
                            this->meshFactory.DestroyResource(*msh);
                        }
                    }
                    this->meshPool.Unassign(id, ResourceState::Initial);
                }
            }
            break;
                
            case GfxResourceType::Shader:
            {
                if (ResourceState::Valid == this->shaderPool.QueryState(id)) {
                    shader* shd = this->shaderPool.Lookup(id);
                    if (shd) {
                        this->shaderFactory.DestroyResource(*shd);
                    }
                }
                this->shaderPool.Unassign(id, ResourceState::Initial);
            }
            break;
                
            case GfxResourceType::ProgramBundle:
            {
                if (ResourceState::Valid == this->programBundlePool.QueryState(id)) {
                    programBundle* prog = this->programBundlePool.Lookup(id);
                    if (prog) {
                        this->programBundleFactory.DestroyResource(*prog);
                    }
                }
                this->programBundlePool.Unassign(id, ResourceState::Initial);
            }
            break;
                
            case GfxResourceType::ConstantBlock:
                o_assert2(false, "FIXME!!!\n");
                break;
                
            case GfxResourceType::DrawState:
            {
                if (ResourceState::Valid == this->drawStatePool.QueryState(id)) {
                    drawState* ds = this->drawStatePool.Lookup(id);
                    if (ds) {
                        this->drawStateFactory.DestroyResource(*ds);
                    }
                }
                this->drawStatePool.Unassign(id, ResourceState::Initial);
            }
            break;
                
            default:
                o_assert(false);
                break;
        }
    }
}
    
//------------------------------------------------------------------------------
/**
    Called once per frame to check whether asynchronously setup 
    resources have finished and to deferred-destroy resources (resources
    can only be destroyed if they are not currently pending for asynchronous
    loading).
*/
void
GfxResourceContainer::UpdatePending() {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    // first check for asynchronously loaded resource that have finished loading
    this->pendingCreateLock.LockWrite();
    for (int32 i = this->pendingCreates.Size() - 1; i >= 0; i--) {
        const Id& resId = this->pendingCreates[i].Key();
        ResourceState::Code state = this->pendingCreates[i].Value();
        o_assert_dbg((ResourceState::Valid == state) || (ResourceState::Failed == state));

        // resource has finished async loading (succeeded or failed),
        // update the resource state in the resource pool
        switch (resId.Type()) {
            case GfxResourceType::Mesh:
                this->meshPool.UpdateState(resId, state);
                break;
            case GfxResourceType::Texture:
                this->texturePool.UpdateState(resId, state);
                break;
            default:
                o_error("Invalid resource type!");
                break;
        }
        // remove entry since it is handled now
        this->pendingCreates.Erase(i);
    }
    this->pendingCreateLock.UnlockWrite();
    
    // next check for resources that need to be destroyed, only resources
    // that are not currently asynchronously loading must be touched
    for (int32 i = this->pendingDestroys.Size() - 1; i >= 0; i--) {
        bool removeMe = false;
        const Id& resId = this->pendingDestroys[i];
        switch (resId.Type()) {
            case GfxResourceType::Texture:
            {
                ResourceState::Code state = this->texturePool.QueryState(resId);
                if (ResourceState::Pending != state) {
                    removeMe = true;
                    if (ResourceState::Valid == state) {
                        texture* tex = this->texturePool.Lookup(resId);
                        if (tex) {
                            this->textureFactory.DestroyResource(*tex);
                        }
                    }
                    this->texturePool.Unassign(resId, ResourceState::Initial);
                }
            }
            break;
            
            case GfxResourceType::Mesh:
            {
                ResourceState::Code state = this->meshPool.QueryState(resId);
                if (ResourceState::Pending != state) {
                    removeMe = true;
                    if (ResourceState::Valid == state) {
                        mesh* msh = this->meshPool.Lookup(resId);
                        if (msh) {
                            this->meshFactory.DestroyResource(*msh);
                        }
                    }
                    this->meshPool.Unassign(resId, ResourceState::Initial);
                }
            }
            break;
                
            default:
                o_assert(false);
                break;
        }
        
        // remove destroy requests that have been handled
        if (removeMe) {
            this->pendingDestroys.Erase(i);
        }
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
GfxResourceContainer::QueryState(const Id& resId) const {
    o_assert_dbg(this->isValid());
    o_assert_dbg(Core::IsMainThread());
    
    switch (resId.Type()) {
        case GfxResourceType::Texture:
            return this->texturePool.QueryState(resId);
        case GfxResourceType::Mesh:
            return this->meshPool.QueryState(resId);
        case GfxResourceType::Shader:
            return this->shaderPool.QueryState(resId);
        case GfxResourceType::ProgramBundle:
            return this->programBundlePool.QueryState(resId);
        case GfxResourceType::ConstantBlock:
            o_assert2(false, "FIXME!!!\n");
            break;
        case GfxResourceType::DrawState:
            return this->drawStatePool.QueryState(resId);
        default:
            o_assert(false);
            break;
    }
    return ResourceState::InvalidState;
}

} // namespace Oryol