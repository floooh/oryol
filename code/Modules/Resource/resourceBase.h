#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Resource::resourceBase
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
#include "Resource/Id.h"
#include "Resource/Locator.h"
#include "Resource/State.h"

namespace Oryol {
namespace Resource {
    
template<class SETUP> class resourceBase {
public:
    /// constructor
    resourceBase();
    /// destructor
    ~resourceBase();
    
    /// get the resource id
    const Id& GetId() const;
    /// get the locator
    const Locator& GetLocator() const;
    /// get the current state of the resource
    State::Code GetState() const;
    /// get the setup object
    const SETUP& GetSetup() const;

    /// set the resource id of the resource
    void setId(const Id& id);
    /// set the resource locator
    void setLocator(const Locator& loc);
    /// set the resource state
    void setState(State::Code s);
    /// set the setup object
    void setSetup(const SETUP& s);
    /// set the loader index (set by factory)
    void setLoaderIndex(int32 i);
    /// get the loader index
    int32 getLoaderIndex() const;
    
protected:
    Id id;
    Locator locator;
    State::Code state;
    SETUP setup;
    int32 loaderIndex;
};

//------------------------------------------------------------------------------
template<class SETUP>
resourceBase<SETUP>::resourceBase() :
state(State::Initial),
loaderIndex(InvalidIndex) {
    // empty
}
    
//------------------------------------------------------------------------------
template<class SETUP>
resourceBase<SETUP>::~resourceBase() {
    o_assert(State::Valid != this->state);
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
resourceBase<SETUP>::setLocator(const Locator& loc) {
    this->locator = loc;
}
    
//------------------------------------------------------------------------------
template<class SETUP> const Locator&
resourceBase<SETUP>::GetLocator() const {
    return this->locator;
}

//------------------------------------------------------------------------------
template<class SETUP> void
resourceBase<SETUP>::setState(State::Code s) {
    this->state = s;
}

//------------------------------------------------------------------------------
template<class SETUP> State::Code
resourceBase<SETUP>::GetState() const {
    return this->state;
}

//------------------------------------------------------------------------------
template<class SETUP> void
resourceBase<SETUP>::setSetup(const SETUP& setup_) {
    this->setup = setup_;
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

} // namespace Render
} // namespace Oryol
 