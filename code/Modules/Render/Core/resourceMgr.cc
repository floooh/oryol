//------------------------------------------------------------------------------
//  resourceMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/base/meshLoaderBase.h"
#include "resourceMgr.h"

namespace Oryol {
namespace Render {

using namespace Core;
using namespace Resource;
    
//------------------------------------------------------------------------------
resourceMgr::resourceMgr() :
isValid(false),
stateWrapper(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
resourceMgr::~resourceMgr() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
template<> void
resourceMgr::AttachLoader(const Ptr<meshLoaderBase>& loader) {
    this->meshFactory.AttachLoader(loader);
}

//------------------------------------------------------------------------------
void
resourceMgr::Setup(const RenderSetup& setup, class stateWrapper* stWrapper) {
    o_assert(!this->isValid);
    o_assert(nullptr != stWrapper);
    
    this->isValid = true;
    this->stateWrapper = stWrapper;

    this->meshFactory.Setup(this->stateWrapper);
    this->meshPool.Setup(&this->meshFactory, setup.GetPoolSize(ResourceType::Mesh), setup.GetThrottling(ResourceType::Mesh), 'MESH');
    this->shaderFactory.Setup();
    this->shaderPool.Setup(&this->shaderFactory, setup.GetPoolSize(ResourceType::Shader), 0, 'SHDR');
    
    this->resourceRegistry.Setup(setup.GetResourceRegistryCapacity());
}

//------------------------------------------------------------------------------
void
resourceMgr::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    this->resourceRegistry.Discard();
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
    this->meshPool.Update();
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const MeshSetup& setup) {
    o_assert(this->isValid);
    const Locator& loc = setup.GetLocator();
    Id resId = this->resourceRegistry.LookupResource(setup.GetLocator());
    if (resId.IsValid()) {
        o_assert(resId.Type() == ResourceType::Mesh);
        return resId;
    }
    else {
        resId = this->meshPool.AllocId();
        this->resourceRegistry.AddResource(loc, resId);
        this->meshPool.Assign(resId, setup);
        return resId;
    }
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const MeshSetup& setup, const Ptr<IO::Stream>& data) {
    o_assert(this->isValid);
    const Locator& loc = setup.GetLocator();
    Id resId = this->resourceRegistry.LookupResource(setup.GetLocator());
    if (!resId.IsValid()) {
        resId = this->meshPool.AllocId();
        this->resourceRegistry.AddResource(loc, resId);
        this->meshPool.Assign(resId, setup, data);
    }
    return resId;
}

//------------------------------------------------------------------------------
template<> Id
resourceMgr::CreateResource(const ShaderSetup& setup) {
    o_assert(this->isValid);
    const Locator& loc = setup.GetLocator();
    Id resId = this->resourceRegistry.LookupResource(setup.GetLocator());
    if (resId.IsValid()) {
        o_assert(resId.Type() == ResourceType::Shader);
        return resId;
    }
    else {
        resId = this->shaderPool.AllocId();
        this->resourceRegistry.AddResource(loc, resId);
        this->shaderPool.Assign(resId, setup);
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
resourceMgr::DiscardResource(const Id& resId) {
    o_assert(this->isValid);
    if (this->resourceRegistry.ReleaseResource(resId, this->removedIds) > 0) {
        // removedIds now has the resources which need to be destroyed
        for (const Id& removeId : this->removedIds) {
            switch (removeId.Type()) {
                case ResourceType::Texture:
                    o_assert2(false, "FIXME!!!\n");
                    break;
                case ResourceType::Mesh:
                    this->meshPool.Unassign(resId);
                    break;
                case ResourceType::Shader:
                    this->shaderPool.Unassign(resId);
                    break;
                case ResourceType::ProgramBundle:
                    o_assert2(false, "FIXME!!!\n");
                    break;
                case ResourceType::StateBlock:
                    o_assert2(false, "FIXME!!!\n");
                    break;
                case ResourceType::ConstantBlock:
                    o_assert2(false, "FIXME!!!\n");
                    break;
                default:
                    o_assert(false);
                    break;
            }
        }
    }
}

//------------------------------------------------------------------------------
State::Code
resourceMgr::QueryResourceState(const Id& resId) const {
    o_assert(this->isValid);
    switch (resId.Type()) {
        case ResourceType::Texture:
            o_assert2(false, "FIXME!!!\n");
            break;
        case ResourceType::Mesh:
            return this->meshPool.QueryState(resId);
            break;
        case ResourceType::Shader:
            return this->shaderPool.QueryState(resId);
        case ResourceType::ProgramBundle:
            o_assert2(false, "FIXME!!!\n");
            break;
        case ResourceType::StateBlock:
            o_assert2(false, "FIXME!!!\n");
            break;
        case ResourceType::ConstantBlock:
            o_assert2(false, "FIXME!!!\n");
            break;
        default:
            o_assert(false);
            break;
    }
}

} // namespace Render
} // namespace Oryol