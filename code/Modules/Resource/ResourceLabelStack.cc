//------------------------------------------------------------------------------
//  ResourceLabelStack.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ResourceLabelStack.h"

namespace Oryol {

//------------------------------------------------------------------------------
ResourceLabelStack::~ResourceLabelStack() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
ResourceLabelStack::Setup(int stackCapacity) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(stackCapacity > 0);
    this->labelStack.Reserve(stackCapacity);
    this->isValid = true;
    this->PushLabel(ResourceLabel::Default);
}

//------------------------------------------------------------------------------
void
ResourceLabelStack::Discard() {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->labelStack.Size() == 1);
    this->PopLabel();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
ResourceLabelStack::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceLabel
ResourceLabelStack::PushLabel() {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->curLabelCount < ResourceLabel::Default);
    this->labelStack.Add(this->curLabelCount++);
    return this->labelStack.Back();
}

//------------------------------------------------------------------------------
void
ResourceLabelStack::PushLabel(ResourceLabel label) {
    o_assert_dbg(this->isValid);
    this->labelStack.Add(label);
}

//------------------------------------------------------------------------------
ResourceLabel
ResourceLabelStack::PopLabel() {
    o_assert_dbg(this->isValid);
    ResourceLabel label = this->labelStack.Back();
    this->labelStack.Erase(this->labelStack.Size() - 1);
    return label;
}

//------------------------------------------------------------------------------
ResourceLabel
ResourceLabelStack::PeekLabel() const {
    o_assert_dbg(this->isValid);
    return this->labelStack.Back();
}

} // namespace Oryol
