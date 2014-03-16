#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Resource::slot
    @brief a slot in a Resource::Pool
*/
#include "Resource/Id.h"
#include "IO/Stream.h"

namespace Oryol {
namespace Resource {
    
template<class RESOURCE, class FACTORY> class slot {
public:
    /// constructor
    slot();
    
    /// assign a resource to the slot
    void Assign(FACTORY* factory, const Id& id, const SETUP& setup);
    /// assign a resource with creation data to the slot
    void Assign(FACTORY* factory, const Id& id, const SETUP& setup, const Ptr<IO::Stream>& data);
    /// test if the Update method needs to be called
    bool NeedsUpdate() const;
    /// update the slot
    void Update(FACTORY* factory);
    /// unassign the resource
    void Unassign(FACTORY* factory);
    
    /// get the Id of the currently assigned resource
    const Id& GetId() const;
    /// get the resource currently assigned to the slot
    const RESOURCE& GetResource() const;
    /// get optional data stream
    const Ptr<IO::Stream>& GetStream() const;
    
private:
    RESOURCE resource;
    Ptr<IO::Stream> stream;
};
    
} // namespace Resource
} // namespace Oryol