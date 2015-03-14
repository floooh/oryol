#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::resourceContainerBase
    @brief base class for resource containers
    
    A resource container manages creation, pooling, querying and
    discard for different types of related resources. Modules like
    the Gfx module typically derive a single ResourceContainer subclass
    to wrap their different resource types.
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"
#include "Resource/Core/resourceRegistry.h"
#include "Resource/ResourceLabel.h"

namespace Oryol {

class resourceContainerBase {
public:
    /// default constructor
    resourceContainerBase();
    /// destructor
    ~resourceContainerBase();
    
    /// generate new resource label and push on label stack
    ResourceLabel PushLabel();
    /// push explicit resource label on label stack
    void PushLabel(ResourceLabel label);
    /// pop resource label from label stack
    ResourceLabel PopLabel();
    /// lookup a resource Id by Locator
    Id Lookup(const Locator& locator) const;
    
protected:
    /// setup the resource container
    void setup(int32 labelStackCapacity, int32 registryCapacity);
    /// discard the resource container
    void discard();
    /// return true if valid
    bool isValid() const;
    /// peek top of label stack
    ResourceLabel peekLabel() const;
    /// return true if currently on main thread
    bool isMainThread() const;
    
    Array<ResourceLabel> labelStack;
    _priv::resourceRegistry registry;
    uint32 curLabelCount;
    bool valid;
};

} // namespace Oryol
