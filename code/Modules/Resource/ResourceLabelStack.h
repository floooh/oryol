#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceLabelStack
    @ingroup Resource
    @brief a stack for resource labels
*/
#include "Core/Containers/Array.h"
#include "Resource/ResourceLabel.h"

namespace Oryol {

class ResourceLabelStack {
public:
    /// destructor
    ~ResourceLabelStack();

    /// setup the label stack
    void Setup(int stackCapacity);
    /// discard the label stack
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;

    /// peek top of label stack
    ResourceLabel PeekLabel() const;
    /// generate new resource label and push on label stack
    ResourceLabel PushLabel();
    /// push explicit resource label on label stack
    void PushLabel(ResourceLabel label);
    /// pop resource label from label stack
    ResourceLabel PopLabel();

    Array<ResourceLabel> labelStack;
    uint32_t curLabelCount = 0;
    bool isValid = false;
};

} // namespace Oryol
