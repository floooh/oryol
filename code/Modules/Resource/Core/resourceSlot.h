#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::resourceSlot
    @ingroup Resource
    @brief a slot in a ResourcePool
*/
#include "Resource/Id.h"
#include "IO/Stream/Stream.h"

namespace Oryol {
    
template<class RESOURCE, class SETUP, class FACTORY> class resourceSlot {
public:
    /// constructor
    resourceSlot();
    
    /// assign a resource to the slot
    void Assign(FACTORY* factory, const Id& id, const SETUP& setup);
    /// assign a resource with creation data to the slot
    void Assign(FACTORY* factory, const Id& id, const SETUP& setup, const Ptr<Stream>& data);
    /// test if the resource is ready for the validate method (asynchronous creation)
    bool ReadyForValidate(FACTORY* factory) const;
    /// validate the slot (asynchronous creation)
    void Validate(FACTORY* factory);
    /// unassign the resource
    void Unassign(FACTORY* factory);
    
    /// get the resource currently assigned to the slot
    RESOURCE& GetResource();
    /// get the resource id of the resource assigned to the slot
    const Id& GetId() const;
    /// return true if the slot currently has no resource assigned
    bool IsAssigned() const;
    /// return true if the slot currently has a resource assigned
    bool IsUnassigned() const;
    /// return true if the slot's resource is in pending state
    bool IsPending() const;
    /// return true if the slot's resource is valid
    bool IsValid() const;
    
private:
    RESOURCE resource;
    Ptr<Stream> stream;
};

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY>
resourceSlot<RESOURCE,SETUP,FACTORY>::resourceSlot() {
    // empty
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> const Id&
resourceSlot<RESOURCE,SETUP,FACTORY>::GetId() const {
    return this->resource.GetId();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> RESOURCE&
resourceSlot<RESOURCE,SETUP,FACTORY>::GetResource() {
    return this->resource;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
resourceSlot<RESOURCE,SETUP,FACTORY>::IsAssigned() const {
    return this->resource.GetState() != ResourceState::Initial;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
resourceSlot<RESOURCE,SETUP,FACTORY>::IsUnassigned() const {
    return this->resource.GetState() == ResourceState::Initial;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
resourceSlot<RESOURCE,SETUP,FACTORY>::IsPending() const {
    return this->resource.GetState() == ResourceState::Pending;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
resourceSlot<RESOURCE,SETUP,FACTORY>::IsValid() const {
    return this->resource.GetState() == ResourceState::Valid;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
resourceSlot<RESOURCE,SETUP,FACTORY>::Assign(FACTORY* factory, const Id& id, const SETUP& setup) {
    o_assert(this->IsUnassigned());
    o_assert(!this->stream.isValid());
    o_assert(factory);

    this->resource.setId(id);
    this->resource.setSetup(setup);
    factory->SetupResource(this->resource);
    o_assert((this->resource.GetState() == ResourceState::Pending) || (this->resource.GetState() == ResourceState::Valid) || (this->resource.GetState() == ResourceState::Failed));
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
resourceSlot<RESOURCE,SETUP,FACTORY>::Assign(FACTORY* factory, const Id& id, const SETUP& setup, const Ptr<Stream>& data) {
    o_assert(this->IsUnassigned());
    o_assert(!this->stream.isValid());
    o_assert(factory);
    
    this->resource.setId(id);
    this->resource.setSetup(setup);
    factory->SetupResource(this->resource, data);
    const ResourceState::Code state = this->resource.GetState();
    o_assert((state == ResourceState::Pending) || (state == ResourceState::Valid) || (state == ResourceState::Failed));
    
    // keep the initialization data around as long as the resource is in pending state
    if (state == ResourceState::Pending) {
        this->stream = data;
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
resourceSlot<RESOURCE,SETUP,FACTORY>::ReadyForValidate(FACTORY* factory) const {
    o_assert(this->IsPending());
    o_assert(factory);
    return factory->NeedsSetupResource(this->resource);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
resourceSlot<RESOURCE,SETUP,FACTORY>::Validate(FACTORY* factory) {
    o_assert(this->IsPending());
    o_assert(factory);
    
    if (this->stream.isValid()) {
        factory->SetupResource(this->resource, this->stream);
    }
    else {
        factory->SetupResource(this->resource);
    }
    o_assert((this->resource.GetState() == ResourceState::Valid) || (this->resource.GetState() == ResourceState::Failed));
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
resourceSlot<RESOURCE,SETUP,FACTORY>::Unassign(FACTORY* factory) {
    o_assert(this->IsAssigned());
    o_assert(factory);
    
    factory->DestroyResource(this->resource);
    o_assert(this->resource.GetState() == ResourceState::Setup);
    this->resource.setState(ResourceState::Initial);
}

} // namespace Oryol
