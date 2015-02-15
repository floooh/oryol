#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::resourceBase
    @ingroup Resource
    @brief private: resource base class
    
    This is the generic Oryol resource base class. Resources usually manage
    some external data object (like textures, vertices, animations, ...)
    which can be shared, can be created synchronously or asynchronously.
    Resources generally live in ResourcePools, and only a maximum number
    of resources of the same type can exist at one time. The creation
    of a resource is controlled by a matching Setup object, which holds
    all information required to create a resource object. A copy of the
    setup object is stored in the resource object, so that the resource
    can be destroyed and re-created if needed.
*/
#include "Core/Assertion.h"
#include "Resource/Id.h"
#include "Resource/ResourceState.h"

namespace Oryol {
    
template<class SETUP> class resourceBase {
public:
    /// constructor
    resourceBase();
    /// destructor
    ~resourceBase();
    
    /// get the resource id
    const Id& GetId() const;
    /// get the current state of the resource
    ResourceState::Code GetState() const;
    /// get the setup object
    const SETUP& GetSetup() const;
    /// get the placeholder type fourcc (only for asynchronously loaded resources)
    uint32 GetPlaceholderType() const;

    /// set the resource id of the resource
    void setId(const Id& id);
    /// set the resource state
    void setState(ResourceState::Code s);
    /// set the setup object
    void setSetup(const SETUP& s);
    /// set the loader index (set by factory)
    void setLoaderIndex(int32 i);
    /// get the loader index
    int32 getLoaderIndex() const;
    /// set placeholder type fourcc
    void setPlaceholderType(uint32 fourcc);
    /// clear the resource
    void clear();
    
protected:
    Id id;
    ResourceState::Code state;
    SETUP setup;
    int32 loaderIndex;
    uint32 placeholderType;
};

//------------------------------------------------------------------------------
template<class SETUP>
resourceBase<SETUP>::resourceBase() :
state(ResourceState::Initial),
loaderIndex(InvalidIndex),
placeholderType(0) {
    // empty
}
    
//------------------------------------------------------------------------------
template<class SETUP>
resourceBase<SETUP>::~resourceBase() {
    o_assert(ResourceState::Valid != this->state);
}
    
//------------------------------------------------------------------------------
template<class SETUP> void
resourceBase<SETUP>::setId(const Id& id_) {
    this->id = id_;
}
    
//------------------------------------------------------------------------------
template<class SETUP> const Id&
resourceBase<SETUP>::GetId() const {
    return this->id;
}

//------------------------------------------------------------------------------
template<class SETUP> void
resourceBase<SETUP>::setState(ResourceState::Code s) {
    this->state = s;
}

//------------------------------------------------------------------------------
template<class SETUP> ResourceState::Code
resourceBase<SETUP>::GetState() const {
    return this->state;
}

//------------------------------------------------------------------------------
template<class SETUP> uint32
resourceBase<SETUP>::GetPlaceholderType() const {
    return this->placeholderType;
}

//------------------------------------------------------------------------------
template<class SETUP> void
resourceBase<SETUP>::setSetup(const SETUP& setup_) {
    o_assert(ResourceState::Initial == this->state);
    this->setup = setup_;
    this->state = ResourceState::Setup;
}

//------------------------------------------------------------------------------
template<class SETUP> const SETUP&
resourceBase<SETUP>::GetSetup() const {
    return this->setup;
}

//------------------------------------------------------------------------------
template<class SETUP> void
resourceBase<SETUP>::setLoaderIndex(int32 i) {
    this->loaderIndex = i;
}

//------------------------------------------------------------------------------
template<class SETUP> int32
resourceBase<SETUP>::getLoaderIndex() const {
    return this->loaderIndex;
}

//------------------------------------------------------------------------------
template<class SETUP> void
resourceBase<SETUP>::setPlaceholderType(uint32 fourcc) {
    this->placeholderType = fourcc;
}

//------------------------------------------------------------------------------
template<class SETUP> void
resourceBase<SETUP>::clear() {
    this->setup = SETUP();
}

} // namespace Oryol

 