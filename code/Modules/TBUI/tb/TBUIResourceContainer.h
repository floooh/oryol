#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TBUIResourceContainer
    @brief in-memory cache for preloaded UI resources
    
    TBUI doesn't support async file loading, thus we need to preload
    all required resources into memory. The interface is similar to
    the GfxResourceContainer.
*/
#include "Core/RunLoop.h"
#include "Resource/Core/resourceContainerBase.h"
#include "Resource/Core/ResourcePool.h"
#include "IO/IOProtocol.h"
#include "TBUI/tb/tbResource.h"
#include "TBUI/TBUISetup.h"
#include "TBUI/tb/tbOryolFile.h"

namespace Oryol {

class TBUIResourceContainer : public resourceContainerBase {
public:
    /// constructor
    TBUIResourceContainer();
    
    /// asynchronously load a resource
    Id Preload(const Locator& locator, const StringAtom& tbResName);
    /// destroy resource by label
    void Destroy(ResourceLabel label);
    /// query resource state
    ResourceState::Code QueryState(Id id) const;
    /// test if all current resources have finished loading
    bool FinishedLoading() const;

private:
    friend class TBUI;
    friend class tb::TBFile;
    
    /// lookup resource data by id (return invalid Ptr if not found or resource not valid)
    Ptr<Stream> lookupById(const Id& resId) const;
    /// lookup resource data by resource name (return invalid Ptr if not found or not resource not valid)
    Ptr<Stream> lookupByResName(const StringAtom& tbResName);
    /// setup the resource container
    void setup(const TBUISetup& setup);
    /// discard the resource container
    void discard();
    /// update pending resources (called per frame from runloop)
    void update();

    RunLoop::Id runLoopId;
    struct idAndRequest {
        Id id;
        Ptr<IOProtocol::Request> ioRequest;
    };
    Array<idAndRequest> pendingIORequests;
    ResourcePool<_priv::tbResource, StringAtom> resPool;
    Map<StringAtom, Id> resNameMap;
};

} // namespace Oryol
