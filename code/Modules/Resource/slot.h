#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Resource::slot
    @brief a slot in a Resource::Pool
*/
#include "Resource/Id.h"
#include "IO/Stream/Stream.h"

namespace Oryol {
namespace Resource {
    
template<class RESOURCE, class SETUP, class FACTORY> class slot {
public:
    /// constructor
    slot();
    
    /// assign a resource to the slot
    void Assign(FACTORY* factory, const Id& id, const SETUP& setup);
    /// assign a resource with creation data to the slot
    void Assign(FACTORY* factory, const Id& id, const SETUP& setup, const Core::Ptr<IO::Stream>& data);
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
    Core::Ptr<IO::Stream> stream;
};

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY>
slot<RESOURCE,SETUP,FACTORY>::slot() {
    // empty
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> const Id&
slot<RESOURCE,SETUP,FACTORY>::GetId() const {
    return this->resource.GetId();
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> RESOURCE&
slot<RESOURCE,SETUP,FACTORY>::GetResource() {
    return this->resource;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
slot<RESOURCE,SETUP,FACTORY>::IsAssigned() const {
    return this->resource.GetState() != State::Initial;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
slot<RESOURCE,SETUP,FACTORY>::IsUnassigned() const {
    return this->resource.GetState() == State::Initial;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
slot<RESOURCE,SETUP,FACTORY>::IsPending() const {
    return this->resource.GetState() == State::Pending;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
slot<RESOURCE,SETUP,FACTORY>::IsValid() const {
    return this->resource.GetState() == State::Valid;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
slot<RESOURCE,SETUP,FACTORY>::Assign(FACTORY* factory, const Id& id, const SETUP& setup) {
    o_assert(this->IsUnassigned());
    o_assert(!this->stream.isValid());
    o_assert(factory);

    this->resource.setId(id);
    this->resource.setSetup(setup);
    factory->SetupResource(this->resource);
    o_assert((this->resource.GetState() == State::Pending) || (this->resource.GetState() == State::Valid) || (this->resource.GetState() == State::Failed));
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
slot<RESOURCE,SETUP,FACTORY>::Assign(FACTORY* factory, const Id& id, const SETUP& setup, const Core::Ptr<IO::Stream>& data) {
    o_assert(this->IsUnassigned());
    o_assert(!this->stream.isValid());
    o_assert(factory);
    
    this->resource.setId(id);
    this->resource.setSetup(setup);
    factory->SetupResource(this->resource, data);
    const State::Code state = this->resource.GetState();
    o_assert((state == State::Pending) || (state == State::Valid) || (state == State::Failed));
    
    // keep the initialization data around as long as the resource is in pending state
    if (state == State::Pending) {
        this->stream = data;
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> bool
slot<RESOURCE,SETUP,FACTORY>::ReadyForValidate(FACTORY* factory) const {
    o_assert(this->IsPending());
    o_assert(factory);
    return factory->NeedsSetupResource(this->resource);
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
slot<RESOURCE,SETUP,FACTORY>::Validate(FACTORY* factory) {
    o_assert(this->IsPending());
    o_assert(factory);
    
    if (this->stream.isValid()) {
        factory->SetupResource(this->resource, this->stream);
    }
    else {
        factory->SetupResource(this->resource);
    }
    o_assert((this->resource.GetState() == State::Valid) || (this->resource.GetState() == State::Failed));
}

//------------------------------------------------------------------------------
template<class RESOURCE, class SETUP, class FACTORY> void
slot<RESOURCE,SETUP,FACTORY>::Unassign(FACTORY* factory) {
    o_assert(this->IsAssigned());
    o_assert(factory);
    
    factory->DestroyResource(this->resource);
    o_assert(this->resource.GetState() == State::Setup);
    this->resource.setState(State::Initial);
}

} // namespace Resource
} // namespace Oryol