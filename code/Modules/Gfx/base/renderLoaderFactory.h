#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::renderLoaderFactory
    @ingroup _priv
    @brief template loader factory base class for rendering resources
*/
#include "Resource/Core/loaderFactory.h"
#include "Gfx/Core/Enums.h"
#include "IO/IOProtocol.h"

namespace Oryol {
namespace _priv {
    
template<class RESOURCE, class RESLOADERBASE, ResourceType::Code TYPE>
class renderLoaderFactory : public loaderFactory<RESOURCE, RESLOADERBASE> {
public:
    /// get the resource type this factory produces
    uint16 GetResourceType() const;
    /// determine whether asynchronous loading has finished
    bool NeedsSetupResource(const RESOURCE& res) const;
    /// destroy the resource
    void DestroyResource(RESOURCE& res);
};

//------------------------------------------------------------------------------
template<class RESOURCE, class RESLOADERBASE, ResourceType::Code TYPE> uint16
renderLoaderFactory<RESOURCE, RESLOADERBASE, TYPE>::GetResourceType() const {
    return TYPE;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class RESLOADERBASE, ResourceType::Code TYPE> bool
renderLoaderFactory<RESOURCE, RESLOADERBASE, TYPE>::NeedsSetupResource(const RESOURCE& res) const {
    o_assert(res.GetState() == ResourceState::Pending);
    const Ptr<IOProtocol::Request>& ioRequest = res.GetIORequest();
    if (ioRequest.isValid()) {
        return ioRequest->Handled();
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class RESLOADERBASE, ResourceType::Code TYPE> void
renderLoaderFactory<RESOURCE, RESLOADERBASE, TYPE>::DestroyResource(RESOURCE& res) {
    // We need to hook in here and check whether there's an asynchronous
    // IORequest in flight. If yes, cancel it so that it doesn't load data
    // which actually isn't needed anymore
    const Ptr<IOProtocol::Request>& ioRequest = res.GetIORequest();
    if (ioRequest.isValid()) {
        ioRequest->SetCancelled();
    }
    loaderFactory<RESOURCE,RESLOADERBASE>::DestroyResource(res);
}

} // namespace _priv
} // namespace Oryol
