#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceContainerBase
    @ingroup Resource
    @brief base class for resource containers
    
    A resource container manages creation, pooling, querying and
    discard for different types of related resources. Modules like
    the Gfx module typically derive a single ResourceContainer subclass
    to wrap their different resource types.
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"
#include "Resource/ResourceRegistry.h"
#include "Resource/ResourceLabel.h"

namespace Oryol {

class ResourceContainerBase {
public:
    /// destructor
    ~ResourceContainerBase();
    
    /// setup the resource container
    void Setup(int labelStackCapacity, int registryCapacity);
    /// discard the resource container
    void Discard();
    /// return true if valid
    bool IsValid() const;
    /// peek top of label stack
    ResourceLabel PeekLabel() const;
    /// generate new resource label and push on label stack
    ResourceLabel PushLabel();
    /// push explicit resource label on label stack
    void PushLabel(ResourceLabel label);
    /// pop resource label from label stack
    ResourceLabel PopLabel();
    /// lookup a resource Id by Locator
    Id Lookup(const Locator& locator) const;
    
    Array<ResourceLabel> labelStack;
    ResourceRegistry registry;
    uint32_t curLabelCount = 0;
    bool valid = false;
};

} // namespace Oryol
