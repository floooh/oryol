#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::simpleFactory
    @ingroup Resource
    @brief base class for factories that don't support loaders
    @see loaderFactory
*/
#include "IO/Stream/Stream.h"
#include "Resource/ResourceState.h"

namespace Oryol {
    
template<class RESOURCE> class simpleFactory {
public:
    /// test if setup should be called for a resource
    bool NeedsSetupResource(const RESOURCE& resource) const;
    /// setup resource, continue calling until res state is not Pending
    void SetupResource(RESOURCE& resource);
    /// setup with input data, continue calling until res state is not Pending
    void SetupResource(RESOURCE& resource, const Ptr<Stream>& data);
    /// destroy the resource
    void DestroyResource(RESOURCE& resource);
};

//------------------------------------------------------------------------------
template<class RESOURCE> bool
simpleFactory<RESOURCE>::NeedsSetupResource(const RESOURCE& res) const {
    // implement in subclass!
    return false;
}

//------------------------------------------------------------------------------
template<class RESOURCE> void
simpleFactory<RESOURCE>::SetupResource(RESOURCE& res) {
    // implement in subclass!
    res.setState(ResourceState::Failed);
}

//------------------------------------------------------------------------------
template<class RESOURCE> void
simpleFactory<RESOURCE>::SetupResource(RESOURCE& res, const Ptr<Stream>& data) {
    // implement in subclass!
    res.setState(ResourceState::Failed);
}

//------------------------------------------------------------------------------
template<class RESOURCE> void
simpleFactory<RESOURCE>::DestroyResource(RESOURCE& res) {
    // implement in subclass!
}

} // namespace Oryol
