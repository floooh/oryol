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

namespace Oryol {
    
template<class SETUP> class resourceBase {
public:
    /// the resource id
    class Id Id;
    /// the setup object
    SETUP Setup;
    
    /// clear the resource
    void Clear();
};

//------------------------------------------------------------------------------
template<class SETUP> void
resourceBase<SETUP>::Clear() {
    this->Setup = SETUP();
}

} // namespace Oryol

 