//------------------------------------------------------------------------------
//  gfxResourceContainerBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "IO/IO.h"
#include "gfxResourceContainerBase.h"
#include "Gfx/Core/displayMgr.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
gfxResourceContainerBase::gfxResourceContainerBase() :
runLoopId(RunLoop::InvalidId) {
    // empty
}

//------------------------------------------------------------------------------
void
gfxResourceContainerBase::setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->isValid());
    
    this->pointers = ptrs;
    
    this->meshFactory.Setup(this->pointers);
    this->meshPool.Setup(GfxResourceType::Mesh, setup.PoolSize(GfxResourceType::Mesh));
    this->shaderFactory.Setup(this->pointers);
    this->shaderPool.Setup(GfxResourceType::Shader, setup.PoolSize(GfxResourceType::Shader));
    this->textureFactory.Setup(this->pointers);
    this->texturePool.Setup(GfxResourceType::Texture, setup.PoolSize(GfxResourceType::Texture));
    this->drawStateFactory.Setup(this->pointers);
    this->drawStatePool.Setup(GfxResourceType::DrawState, setup.PoolSize(GfxResourceType::DrawState));
    this->textureBlockFactory.Setup(this->pointers);
    this->textureBlockPool.Setup(GfxResourceType::TextureBlock, setup.PoolSize(GfxResourceType::TextureBlock));
    
    this->runLoopId = Core::PostRunLoop()->Add([this]() {
        this->update();
    });
    
    resourceContainerBase::setup(setup.ResourceLabelStackCapacity, setup.ResourceRegistryCapacity);
}

//------------------------------------------------------------------------------
void
gfxResourceContainerBase::discard() {
    o_assert_dbg(this->isValid());
    
    Core::PostRunLoop()->Remove(this->runLoopId);
    for (const auto& loader : this->pendingLoaders) {
        loader->Cancel();
    }
    this->pendingLoaders.Clear();
    
    resourceContainerBase::discard();

    this->textureBlockPool.Discard();
    this->textureBlockFactory.Discard();
    this->drawStatePool.Discard();
    this->drawStateFactory.Discard();
    this->texturePool.Discard();
    this->textureFactory.Discard();
    this->shaderPool.Discard();
    this->shaderFactory.Discard();
    this->meshPool.Discard();
    this->meshFactory.Discard();
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
template<> Id
gfxResourceContainerBase::Create(const MeshSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        mesh& res = this->meshPool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->meshFactory.SetupResource(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->meshPool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
gfxResourceContainerBase::Create(const MeshSetup& setup, const void* data, int32 size) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->meshPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        mesh& res = this->meshPool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->meshFactory.SetupResource(res, data, size);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->meshPool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
gfxResourceContainerBase::Create(const TextureSetup& setup) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        texture& res = this->texturePool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->textureFactory.SetupResource(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->texturePool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
gfxResourceContainerBase::Create(const TextureSetup& setup, const void* data, int32 size) {
    o_assert_dbg(this->isValid());
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);
    o_assert_dbg(!setup.ShouldSetupFromFile());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->texturePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        texture& res = this->texturePool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->textureFactory.SetupResource(res, data, size);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->texturePool.UpdateState(resId, newState);
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
gfxResourceContainerBase::prepareAsync(const MeshSetup& setup) {
    o_assert_dbg(this->isValid());
    
    Id resId = this->meshPool.AllocId();
    this->registry.Add(setup.Locator, resId, this->peekLabel());
    this->meshPool.Assign(resId, setup, ResourceState::Pending);
    return resId;
}

//------------------------------------------------------------------------------
template<> ResourceState::Code 
gfxResourceContainerBase::initAsync(const Id& resId, const MeshSetup& setup, const void* data, int32 size) {
    o_assert_dbg(this->isValid());
    
    // the prepared resource may have been destroyed while it was loading
    if (this->meshPool.Contains(resId)) {
        mesh& res = this->meshPool.Assign(resId, setup, ResourceState::Pending);
        const ResourceState::Code newState = this->meshFactory.SetupResource(res, data, size);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->meshPool.UpdateState(resId, newState);
        return newState;
    }
    else {
        // the prepared mesh object was destroyed before it was loaded
        o_warn("gfxResourceContainer::initAsync(): resource destroyed before initAsync (type: %d, slot: %d!)\n",
            resId.Type, resId.SlotIndex);
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
template<> Id
gfxResourceContainerBase::prepareAsync(const TextureSetup& setup) {
    o_assert_dbg(this->isValid());
    
    Id resId = this->texturePool.AllocId();
    this->registry.Add(setup.Locator, resId, this->peekLabel());
    this->texturePool.Assign(resId, setup, ResourceState::Pending);
    return resId;
}

//------------------------------------------------------------------------------
template<> ResourceState::Code 
gfxResourceContainerBase::initAsync(const Id& resId, const TextureSetup& setup, const void* data, int32 size) {
    o_assert_dbg(this->isValid());
    
    // the prepared resource may have been destroyed while it was loading
    if (this->texturePool.Contains(resId)) {
        texture& res = this->texturePool.Assign(resId, setup, ResourceState::Pending);
        const ResourceState::Code newState = this->textureFactory.SetupResource(res, data, size);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->texturePool.UpdateState(resId, newState);
        return newState;
    }
    else {
        // the prepared texture object was destroyed before it was loaded
        o_warn("gfxResourceContainer::initAsync(): resource destroyed before initAsync (type: %d, slot: %d!)\n",
            resId.Type, resId.SlotIndex);
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
gfxResourceContainerBase::failedAsync(const Id& resId) {
    o_assert_dbg(this->isValid());
    
    switch (resId.Type) {
        case GfxResourceType::Mesh:
            // the prepared resource may have been destroyed while it was loading
            if (this->meshPool.Contains(resId)) {
                this->meshPool.UpdateState(resId, ResourceState::Failed);
                return ResourceState::Failed;
            }
            break;
            
        case GfxResourceType::Texture:
            // the prepared resource may have been destroyed while it was loading
            if (this->texturePool.Contains(resId)) {
                this->texturePool.UpdateState(resId, ResourceState::Failed);
                return ResourceState::Failed;
            }
            break;
            
        default:
            o_error("Invalid resource type for async creation!");
            break;
    }
    // fallthrough: resource was already destroyed while still loading
    return ResourceState::InvalidState;
}


//------------------------------------------------------------------------------
template<> Id
gfxResourceContainerBase::Create(const ShaderSetup& setup) {
    o_assert_dbg(this->isValid());
    
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->shaderPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        shader& res = this->shaderPool.Assign(resId, setup, ResourceState::Setup);
        const ResourceState::Code newState = this->shaderFactory.SetupResource(res);
        o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
        this->shaderPool.UpdateState(resId, newState);
    }
    return resId;
}
    
//------------------------------------------------------------------------------
template<> Id
gfxResourceContainerBase::Create(const DrawStateSetup& setup) {
    o_assert_dbg(this->isValid());
    
    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->drawStatePool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        drawState& res = this->drawStatePool.Assign(resId, setup, ResourceState::Setup);

        // check if all referenced resources are loaded, if not defer draw state creation
        const ResourceState::Code resState = this->queryDrawStateDepState(&res);
        if (resState == ResourceState::Pending) {
            this->pendingDrawStates.Add(resId);
            this->drawStatePool.UpdateState(resId, ResourceState::Pending);
        }
        else {
            const ResourceState::Code newState = this->drawStateFactory.SetupResource(res);
            o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));        
            this->drawStatePool.UpdateState(resId, newState);
        }
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
gfxResourceContainerBase::Create(const TextureBlockSetup& setup) {
    o_assert_dbg(this->isValid());

    Id resId = this->registry.Lookup(setup.Locator);
    if (resId.IsValid()) {
        return resId;
    }
    else {
        resId = this->textureBlockPool.AllocId();
        this->registry.Add(setup.Locator, resId, this->peekLabel());
        textureBlock& res = this->textureBlockPool.Assign(resId, setup, ResourceState::Setup);

        // check if all referenced resources are loaded, if not defer creation
        const ResourceState::Code resState = this->queryTextureBlockDepState(&res);
        if (resState == ResourceState::Pending) {
            this->pendingTextureBlocks.Add(resId);
            this->textureBlockPool.UpdateState(resId, ResourceState::Pending);
        }
        else {
            const ResourceState::Code newState = this->textureBlockFactory.SetupResource(res);
            o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));
            this->textureBlockPool.UpdateState(resId, newState);
        }
    }
    return resId;
}

//------------------------------------------------------------------------------
bool
gfxResourceContainerBase::checkState(ResourceState::Code resState) const {
    // helper function for the queryXxxState functions, returns true
    // if the input state is Failed or Pending, fails hard on Initial or
    // Setup state, otherwise returns false
    switch (resState) {
        case ResourceState::Failed:
        case ResourceState::Pending:
            return true;
        case ResourceState::Initial:
        case ResourceState::Setup:
            o_error("gfxResourceContainerBase::checkState: can't happen!");
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
gfxResourceContainerBase::queryDrawStateDepState(const drawState* ds) {
    o_assert_dbg(ds);

    // this returns an overall state of the meshes attached to
    // a draw state (failed, pending, valid)
    for (const Id& meshId : ds->Setup.Meshes) {
        if (meshId.IsValid()) {
            const mesh* msh = this->meshPool.Get(meshId);
            if (msh) {
                if (this->checkState(msh->State)) {
                    // Failed or Pending, exit early
                    return msh->State;
                }
            }
            else {
                // the dependent resource no longer exists
                return ResourceState::Failed;
            }
        }
    }
    // fallthrough means: all mesh dependencies are valid
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
gfxResourceContainerBase::handlePendingDrawStates() {

    // this goes through all pending draw states (where meshes are still
    // loading), checks whether meshes have finished loading (or failed to load)
    // and finishes the draw state creation
    for (int i = this->pendingDrawStates.Size() - 1; i >= 0; i--) {
        Id resId = this->pendingDrawStates[i];
        o_assert_dbg(resId.IsValid());
        drawState* ds = this->drawStatePool.Get(resId);
        if (ds) {
            const ResourceState::Code state = this->queryDrawStateDepState(ds);
            if (state != ResourceState::Pending) {
                if (state == ResourceState::Valid) {
                    // all ok, can setup draw state
                    const ResourceState::Code newState = this->drawStateFactory.SetupResource(*ds);
                    o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));        
                    this->drawStatePool.UpdateState(resId, newState);
                }
                else {
                    // oops, a mesh has failed loading, also set draw state to failed
                    o_assert_dbg(state == ResourceState::Failed);
                    this->drawStatePool.UpdateState(resId, state);
                }
                this->pendingDrawStates.Erase(i);
            }
        }
        else {
            // the drawState object was destroyed before everything was loaded,
            // still need to remove the entry from the pending array
            o_warn("gfxResourceContainer::handlePendingDrawStates(): drawState destroyed before dependencies loaded!\n");
            this->pendingDrawStates.Erase(i);
        }
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
gfxResourceContainerBase::queryTextureBlockDepState(const textureBlock* tb) {
    o_assert_dbg(tb);

    // this returns an overall state of the dependent textures
    for (const Id& texId : tb->Setup.VS) {
        if (texId.IsValid()) {
            const texture* tex = this->texturePool.Get(texId);
            if (tex) {
                if (this->checkState(tex->State)) {
                    // Failed or Pending, exit early
                    return tex->State;
                }
            }
            else {
                // the dependent resource no longer exists
                return ResourceState::Failed;
            }
        }
    }
    for (const Id& texId : tb->Setup.FS) {
        if (texId.IsValid()) {
            const texture* tex = this->texturePool.Get(texId);
            if (tex) {
                if (this->checkState(tex->State)) {
                    // Failed or Pending, exit early
                    return tex->State;
                }
            }
            else {
                // the dependent resource no longer exists
                return ResourceState::Failed;
            }
        }
    }
    // fallthrough means: all texture deps are valid
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
gfxResourceContainerBase::handlePendingTextureBlocks() {
    // goes through the array of pending textureBlocks and checks
    // if their dependent textures have finished loading, if yes,
    // setup the texture block
    for (int i = this->pendingTextureBlocks.Size() - 1; i >= 0; i--) {
        Id resId = this->pendingTextureBlocks[i];
        o_assert_dbg(resId.IsValid());
        textureBlock* tb = this->textureBlockPool.Get(resId);
        if (tb) {
            const ResourceState::Code state = this->queryTextureBlockDepState(tb);
            if (state != ResourceState::Pending) {
                if (state == ResourceState::Valid) {
                    // all ok, can setup texture block
                    const ResourceState::Code newState = this->textureBlockFactory.SetupResource(*tb);
                    o_assert((newState == ResourceState::Valid) || (newState == ResourceState::Failed));        
                    this->textureBlockPool.UpdateState(resId, newState);
                }
                else {
                    // a texture has failed loading, also set textureBlock to failed
                    o_assert_dbg(state == ResourceState::Failed);
                    this->textureBlockPool.UpdateState(resId, state);
                }
                this->pendingTextureBlocks.Erase(i);
            }
        }
        else {
            // the textureBlock object was destroyed before everything was loaded,
            // still need to remove the entry from the pending array
            o_warn("gfxResourceContainer::handlePendingTextureBlocks(): textureBlock destroyed before dependencies loaded!\n");
            this->pendingTextureBlocks.Erase(i);
        }
    }
}

//------------------------------------------------------------------------------
Id
gfxResourceContainerBase::Load(const Ptr<ResourceLoader>& loader) {
    o_assert_dbg(this->isValid());

    Id resId = this->registry.Lookup(loader->Locator());
    if (resId.IsValid()) {
        return resId;
    }
    else {
        this->pendingLoaders.Add(loader);
        resId = loader->Start();
        return resId;
    }
}

//------------------------------------------------------------------------------
void
gfxResourceContainerBase::Destroy(ResourceLabel label) {
    o_assert_dbg(this->isValid());
    
    Array<Id> ids = this->registry.Remove(label);
    for (const Id& id : ids) {
        switch (id.Type) {
            case GfxResourceType::Texture:
            {
                if (ResourceState::Valid == this->texturePool.QueryState(id)) {
                    texture* tex = this->texturePool.Lookup(id);
                    if (tex) {
                        this->textureFactory.DestroyResource(*tex);
                    }
                }
                this->texturePool.Unassign(id);
            }
            break;
                
            case GfxResourceType::Mesh:
            {
                if (ResourceState::Valid == this->meshPool.QueryState(id)) {
                    mesh* msh = this->meshPool.Lookup(id);
                    if (msh) {
                        this->meshFactory.DestroyResource(*msh);
                    }
                }
                this->meshPool.Unassign(id);
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
                this->shaderPool.Unassign(id);
            }
            break;
                
            case GfxResourceType::DrawState:
            {
                if (ResourceState::Valid == this->drawStatePool.QueryState(id)) {
                    drawState* ds = this->drawStatePool.Lookup(id);
                    if (ds) {
                        this->drawStateFactory.DestroyResource(*ds);
                    }
                }
                this->drawStatePool.Unassign(id);
            }
            break;

            case GfxResourceType::TextureBlock:
            {
                if (ResourceState::Valid == this->textureBlockPool.QueryState(id)) {
                    textureBlock* tb = this->textureBlockPool.Lookup(id);
                    if (tb) {
                        this->textureBlockFactory.DestroyResource(*tb);
                    }
                }
                this->textureBlockPool.Unassign(id);
            }
            break;
                
            default:
                o_assert(false);
                break;
        }
    }
}
    
//------------------------------------------------------------------------------
void
gfxResourceContainerBase::update() {
    o_assert_dbg(this->isValid());
    
    /// call update method on resource pools (this is cheap)
    this->meshPool.Update();
    this->shaderPool.Update();
    this->texturePool.Update();
    this->drawStatePool.Update();
    this->textureBlockPool.Update();

    // trigger loaders, and remove from pending array if finished
    for (int32 i = this->pendingLoaders.Size() - 1; i >= 0; i--) {
        const auto& loader = this->pendingLoaders[i];
        ResourceState::Code state = loader->Continue();
        if (ResourceState::Pending != state) {
            this->pendingLoaders.Erase(i);
        }
    }

    // handle drawStates and textureBlocks with pending dependendies
    this->handlePendingDrawStates();
    this->handlePendingTextureBlocks();
}

//------------------------------------------------------------------------------
ResourceInfo
gfxResourceContainerBase::QueryResourceInfo(const Id& resId) const {
    o_assert_dbg(this->isValid());
    
    switch (resId.Type) {
        case GfxResourceType::Texture:
            return this->texturePool.QueryResourceInfo(resId);
        case GfxResourceType::Mesh:
            return this->meshPool.QueryResourceInfo(resId);
        case GfxResourceType::Shader:
            return this->shaderPool.QueryResourceInfo(resId);
        case GfxResourceType::DrawState:
            return this->drawStatePool.QueryResourceInfo(resId);
        case GfxResourceType::TextureBlock:
            return this->textureBlockPool.QueryResourceInfo(resId);
        default:
            o_assert(false);
            return ResourceInfo();
    }
}

//------------------------------------------------------------------------------
ResourcePoolInfo
gfxResourceContainerBase::QueryPoolInfo(GfxResourceType::Code resType) const {
    o_assert_dbg(this->isValid());
    
    switch (resType) {
        case GfxResourceType::Texture:
            return this->texturePool.QueryPoolInfo();
        case GfxResourceType::Mesh:
            return this->meshPool.QueryPoolInfo();
        case GfxResourceType::Shader:
            return this->shaderPool.QueryPoolInfo();
        case GfxResourceType::DrawState:
            return this->drawStatePool.QueryPoolInfo();
        case GfxResourceType::TextureBlock:
            return this->textureBlockPool.QueryPoolInfo();
        default:
            o_assert(false);
            return ResourcePoolInfo();
    }
}

//------------------------------------------------------------------------------
int32
gfxResourceContainerBase::QueryFreeSlots(GfxResourceType::Code resourceType) const {
    o_assert_dbg(this->isValid());

    switch (resourceType) {
        case GfxResourceType::Texture:
            return this->texturePool.GetNumFreeSlots();
        case GfxResourceType::Mesh:
            return this->meshPool.GetNumFreeSlots();
        case GfxResourceType::Shader:
            return this->shaderPool.GetNumFreeSlots();
        case GfxResourceType::DrawState:
            return this->drawStatePool.GetNumFreeSlots();
        case GfxResourceType::TextureBlock:
            return this->textureBlockPool.GetNumFreeSlots();
        default:
            o_assert(false);
            return 0;
    }
}

} // namespace _priv
} // namespace Oryol
