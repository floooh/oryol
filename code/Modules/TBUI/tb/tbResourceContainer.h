#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::tbResourceContainer
    @ingroup _priv
    @brief in-memory cache for preloaded UI resources
    
    TBUI doesn't support async file loading, thus we need to preload
    all required resources into memory. The interface is similar to
    the gfxResourceContainer.
*/
#include "Resource/Core/resourceContainerBase.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/Queue.h"
#include "Core/Containers/Buffer.h"
#include "TBUI/TBUISetup.h"

namespace tb {
class TBFile;
};

namespace Oryol {
namespace _priv {

class tbResourceContainer : public resourceContainerBase {
public:
    /// constructor
    tbResourceContainer();
    
    /// setup the resource container
    void setup(const TBUISetup& setup);
    /// discard the resource container
    void discard();
    /// lookup resource by locator, return nullptr if not found
    Buffer* lookupResource(const Locator& loc);
    /// add a resource buffer object (URL must be set!)
    void add(const Locator& loc, Buffer&& data);

private:
    int uniqueCounter;
    Array<Buffer> resPool;
};

} // namespace _priv
} // namespace Oryol
