//------------------------------------------------------------------------------
//  resourceMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/base/meshLoaderBase.h"
#include "Render/base/textureLoaderBase.h"
#include "resourceMgr.h"

namespace Oryol {
namespace Render {

using namespace Core;
using namespace Resource;
    
//------------------------------------------------------------------------------
resourceMgr::resourceMgr() :
isValid(false),
stateWrapper(nullptr),
displayMgr(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
resourceMgr::~resourceMgr() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
resourceMgr::AttachLoader(meshLoaderBase* loader) {
    this->meshFactory.AttachLoader(loader);
}

//------------------------------------------------------------------------------
void
resourceMgr::AttachLoader(textureLoaderBase* loader) {
    this->textureFactory.AttachLoader(loader);
}
    
//------------------------------------------------------------------------------
void
resourceMgr::Setup(const RenderSetup& setup, class stateWrapper* stWrapper, class displayMgr* dspMgr) {
    o_assert(!this->isValid);
    o_assert(nullptr != stWrapper);
    o_assert(nullptr != dspMgr);
    
    this->isValid = true;
    this->stateWrapper = stWrapper;
    this->displayMgr = dspMgr;

    this->meshFactory.Setup(this->stateWrapper, &this->meshPool);
    this->meshPool.Setup(&this->meshFactory, setup.PoolSize(ResourceType::Mesh), setup.Throttling(ResourceType::Mesh), 'MESH');
    this->shaderFactory.Setup();
    this->shaderPool.Setup(&this->shaderFactory, setup.PoolSize(ResourceType::Shader), 0, 'SHDR');
    this->programBundleFactory.Setup(this->stateWrapper, &this->shaderPool, &this->shaderFactory);
    this->programBundlePool.Setup(&this->programBundleFactory, setup.PoolSize(ResourceType::ProgramBundle), 0, 'PRGB');
    this->textureFactory.Setup(this->stateWrapper, this->displayMgr, &this->texturePool);
    this->texturePool.Setup(&this->textureFactory, setup.PoolSize(ResourceType::Texture), setup.Throttling(ResourceType::Texture), 'TXTR');
    this->drawStateFactory.Setup(&this->meshPool, &this->programBundlePool);
    this->drawStatePool.Setup(&this->drawStateFactory, setup.PoolSize(ResourceType::DrawState), 0, 'DRWS');
    
    this->resourceRegistry.Setup(setup.ResourceRegistryCapacity());
}

//------------------------------------------------------------------------------
void
resourceMgr::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    this->resourceRegistry.Discard();
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
    this->stateWrapper = nullptr;
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
    o_assert(this->isValid);
    Id resId = this->resourceRegistry.LookupResource(setup.Locator);
    if (resId.IsValid()) {
        o_assert(resId.Type() == ResourceType::Mesh);
        return resId;
    }
    else {
        resId = this->meshPool.AllocId();
        Array<Id> deps;
        if (setup.InstanceMesh.IsValid()) {
            deps.AddBack(setup.InstanceMesh);
        }
        this->resourceRegistry.AddResource(setup.Locator, resId, deps);
        this->meshPool.Assign(resId, setup);
        return resId;
    }
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const MeshSetup& setup, const Ptr<IO::Stream>& data) {
    o_assert(this->isValid);
    Id resId = this->resourceRegistry.LookupResource(setup.Locator);
    if (resId.IsValid()) {
        o_assert(resId.Type() == ResourceType::Mesh);
        return resId;
    }
    else {
        resId = this->meshPool.AllocId();
        Array<Id> deps;
        if (setup.InstanceMesh.IsValid()) {
            deps.AddBack(setup.InstanceMesh);
        }
        this->resourceRegistry.AddResource(setup.Locator, resId, deps);
        this->meshPool.Assign(resId, setup, data);
        return resId;
    }
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const TextureSetup& setup) {
    o_assert(this->isValid);
    const Locator& loc = setup.Locator;
    Id resId = this->resourceRegistry.LookupResource(loc);
    if (resId.IsValid()) {
        o_assert(resId.Type() == ResourceType::Texture);
        return resId;
    }
    else {
        resId = this->texturePool.AllocId();
        this->resourceRegistry.AddResource(loc, resId);
        this->texturePool.Assign(resId, setup);
        return resId;
    }
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const TextureSetup& setup, const Ptr<IO::Stream>& data) {
    o_assert(this->isValid);
    const Locator& loc = setup.Locator;
    Id resId = this->resourceRegistry.LookupResource(loc);
    if (resId.IsValid()) {
        o_assert(resId.Type() == ResourceType::Texture);
        return resId;
    }
    else {
        resId = this->texturePool.AllocId();
        this->resourceRegistry.AddResource(loc, resId);
        this->texturePool.Assign(resId, setup, data);
        return resId;
    }
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const ShaderSetup& setup) {
    o_assert(this->isValid);
    Id resId = this->resourceRegistry.LookupResource(setup.Locator);
    if (resId.IsValid()) {
        o_assert(resId.Type() == ResourceType::Shader);
        return resId;
    }
    else {
        resId = this->shaderPool.AllocId();
        this->resourceRegistry.AddResource(setup.Locator, resId);
        this->shaderPool.Assign(resId, setup);
        return resId;
    }
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const ProgramBundleSetup& setup) {
    o_assert(this->isValid);
    Id resId = this->resourceRegistry.LookupResource(setup.Locator);
    if (resId.IsValid()) {
        o_assert(resId.Type() == ResourceType::ProgramBundle);
        return resId;
    }
    else {
        resId = this->programBundlePool.AllocId();
        // add vertex/fragment shaders as dependencies
        Array<Id> deps;
        const int32 numProgs = setup.NumPrograms();
        deps.Reserve(numProgs * 2);
        for (int32 i = 0; i < numProgs; i++) {
            if (setup.VertexShader(i).IsValid()) {
                deps.AddBack(setup.VertexShader(i));
            }
            if (setup.FragmentShader(i).IsValid()) {
                deps.AddBack(setup.FragmentShader(i));
            }
        }
        this->resourceRegistry.AddResource(setup.Locator, resId, deps);
        this->programBundlePool.Assign(resId, setup);
        return resId;
    }
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const DrawStateSetup& setup) {
    o_assert(this->isValid);
    Id resId = this->resourceRegistry.LookupResource(setup.Locator);
    if (resId.IsValid()) {
        o_assert(resId.Type() == ResourceType::ProgramBundle);
        return resId;
    }
    else {
        resId = this->drawStatePool.AllocId();
        // add dependent resources
        Array<Id> deps;
        deps.AddBack(setup.Program);
        deps.AddBack(setup.Mesh);
        this->resourceRegistry.AddResource(setup.Locator, resId, deps);
        this->drawStatePool.Assign(resId, setup);
        return resId;
    }
}
    
//------------------------------------------------------------------------------
Id
resourceMgr::LookupResource(const Locator& loc) {
    o_assert(this->isValid);
    return this->resourceRegistry.LookupResource(loc);
}

//------------------------------------------------------------------------------
void
resourceMgr::ReleaseResource(const Id& resId) {
    o_assert(this->isValid);
    if (this->resourceRegistry.ReleaseResource(resId, this->removedIds) > 0) {
        // removedIds now has the resources which need to be destroyed
        for (const Id& removeId : this->removedIds) {
            switch (removeId.Type()) {
                case ResourceType::Texture:
                    this->texturePool.Unassign(removeId);
                    break;
                case ResourceType::Mesh:
                    this->meshPool.Unassign(removeId);
                    break;
                case ResourceType::Shader:
                    this->shaderPool.Unassign(removeId);
                    break;
                case ResourceType::ProgramBundle:
                    this->programBundlePool.Unassign(removeId);
                    break;
                case ResourceType::ConstantBlock:
                    o_assert2(false, "FIXME!!!\n");
                    break;
                case ResourceType::DrawState:
                    this->drawStatePool.Unassign(removeId);
                    break;
                default:
                    o_assert(false);
                    break;
            }
        }
    }
}

//------------------------------------------------------------------------------
Resource::State::Code
resourceMgr::QueryResourceState(const Id& resId) {
    o_assert(this->isValid);
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
    return Resource::State::InvalidState;
}

} // namespace Render
} // namespace Oryol