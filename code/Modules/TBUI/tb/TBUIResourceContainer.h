#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TBUIResourceContainer
    @brief in-memory cache for preloaded UI resources
    
    TBUI doesn't support async file loading, thus we need to preload
    all required resources into memory. The interface is similar to
    the GfxResourceContainer.
*/
#include "Resource/Core/resourceContainerBase.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Queue.h"
#include "IO/Stream/Stream.h"
#include "TBUI/TBUISetup.h"

namespace tb {
class TBFile;
};

namespace Oryol {

class TBUIResourceContainer : public resourceContainerBase {
public:
    /// constructor
    TBUIResourceContainer();
    
    /// add a resource stream (URL must be set!)
    void Add(const Ptr<Stream>& data);
    /// remove resources by label
    void Remove(ResourceLabel label);

private:
    friend class TBUI;
    friend class tb::TBFile;

    /// setup the resource container
    void setup(const TBUISetup& setup);
    /// discard the resource container
    void discard();
    /// lookup resource by locator
    Ptr<Stream> lookupResource(const Locator& loc);

    int32 uniqueCounter;
    Array<Ptr<Stream>> resPool;
    Queue<uint16> freeSlots;
};

} // namespace Oryol
