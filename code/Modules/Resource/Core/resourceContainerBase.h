#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::resourceContainerBase
    @brief base class for resource containers
    
    A resource container manages creation, pooling, querying and
    discard for different types of related resources. Modules like
    the Gfx module typically derive a single ResourceContainer subclass
    to wrap their different resource types.
    
    A resource container subclass should offer the following interface:
    
    void AttachLoader(const Ptr<ResourceLoader>& loader);
    void DetachLoader(const Ptr<ResourceLoader>& loader);
    void PushLabel(uint8 label);
    uint8 PopLabel();
    template<SETUP> Id Create(const SETUP& setup);
    template<SETUP> Id Create(const SetupAndStream<SETUP> setupAndStream);
    template<SETUP> Id Load(const SETUP& setup);
    Id Lookup(const Locator& locator);
    ResourceState::Code QueryState(const Id& id);
    void Destroy(uint8 label);
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"
#include "Resource/Core/resourceRegistry.h"

namespace Oryol {

class resourceContainerBase {
public:
    /// default constructor
    resourceContainerBase();
    /// destructor
    ~resourceContainerBase();
    
    /// generate new resource label and push on label stack
    uint8 PushLabel();
    /// push explicit resource label on label stack
    void PushLabel(uint8 label);
    /// pop resource label from label stack
    uint8 PopLabel();
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
    uint8 peekLabel() const;
    /// return true if currently on main thread
    bool isMainThread() const;
    
    Array<uint8> labelStack;
    _priv::resourceRegistry registry;
    uint8 curLabelCount;
    bool valid;
};

} // namespace Oryol
