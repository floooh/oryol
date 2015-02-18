//------------------------------------------------------------------------------
//  ResourceLabelStack.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ResourceLabelStack.h"

namespace Oryol {

//------------------------------------------------------------------------------
ResourceLabelStack::ResourceLabelStack() {
    this->stack.Reserve(InitialCapacity);
}

//------------------------------------------------------------------------------
void
ResourceLabelStack::Push(uint8 label) {
    this->stack.Add(label);
}

//------------------------------------------------------------------------------
uint8
ResourceLabelStack::Pop() {
    o_assert_dbg(!this->stack.IsEmpty());
    uint8 label = this->stack.Back();
    this->stack.Erase(this->stack.Size() - 1);
    return label;
}

//------------------------------------------------------------------------------
uint8
ResourceLabelStack::Peek() const {
    o_assert_dbg(!this->stack.IsEmpty());
    return this->stack.Back();
}

//------------------------------------------------------------------------------
int32
ResourceLabelStack::Size() const {
    return this->stack.Size();
}
   
} // namespace Oryol