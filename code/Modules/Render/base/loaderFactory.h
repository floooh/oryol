#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::loaderFactory
    @brief template loader factory base class for rendering resources
*/
#include "Resource/loaderFactory.h"
#include "Render/Core/Enums.h"
#include "IO/IOProtocol.h"

namespace Oryol {
namespace Render {
    
template<class RESOURCE, class RESLOADERBASE, ResourceType::Code TYPE>
class loaderFactory : public Resource::loaderFactory<RESOURCE, RESLOADERBASE> {
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
loaderFactory<RESOURCE, RESLOADERBASE, TYPE>::GetResourceType() const {
    return TYPE;
}

//------------------------------------------------------------------------------
template<class RESOURCE, class RESLOADERBASE, ResourceType::Code TYPE> bool
loaderFactory<RESOURCE, RESLOADERBASE, TYPE>::NeedsSetupResource(const RESOURCE& res) const {
    o_assert(res.GetState() == Resource::State::Pending);
    const Core::Ptr<IO::IOProtocol::Request>& ioRequest = res.GetIORequest();
    if (ioRequest.isValid()) {
        return ioRequest->Handled();
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
template<class RESOURCE, class RESLOADERBASE, ResourceType::Code TYPE> void
loaderFactory<RESOURCE, RESLOADERBASE, TYPE>::DestroyResource(RESOURCE& res) {
    // We need to hook in here and check whether there's an asynchronous
    // IORequest in flight. If yes, cancel it so that it doesn't load data
    // which actually isn't needed anymore
    const Core::Ptr<IO::IOProtocol::Request>& ioRequest = res.GetIORequest();
    if (ioRequest.isValid()) {
        ioRequest->SetCancelled();
    }
    Resource::loaderFactory<RESOURCE,RESLOADERBASE>::DestroyResource(res);
}

} // namespace Render
} // namespace Oryol