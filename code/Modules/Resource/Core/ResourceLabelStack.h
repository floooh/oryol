#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceLabelStack
    @ingroup Resource
    @brief push, pop and peek resource labels
*/
#include "Core/Containers/Array.h"

namespace Oryol {

class ResourceLabelStack {
public:
    /// constructor
    ResourceLabelStack();
    
    /// push a resource label
    void Push(uint8 label);
    /// pop a resource label
    uint8 Pop();
    /// peek at top label
    uint8 Peek() const;
    /// number of elements in stack
    int32 Size() const;
    
private:
    static const int32 InitialCapacity = 256;
    Array<uint8> stack;
};

} // namespace Oryol