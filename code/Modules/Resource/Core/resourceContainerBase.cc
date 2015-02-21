//------------------------------------------------------------------------------
//  resourceContainerBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "resourceContainerBase.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
resourceContainerBase::resourceContainerBase() :
valid(false) {
    // empty
}

//------------------------------------------------------------------------------
resourceContainerBase::~resourceContainerBase() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
resourceContainerBase::setup(int32 labelStackCapacity, int32 registryCapacity) {
    o_assert_dbg(!this->valid);
    this->labelStack.Reserve(labelStackCapacity);
    this->registry.Setup(registryCapacity);
    this->valid = true;
    this->PushLabel(Id::LabelDefault);
}

//------------------------------------------------------------------------------
void
resourceContainerBase::discard() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->labelStack.Size() == 1);
    uint8 defLabel = this->PopLabel();
    o_assert_dbg(Id::LabelDefault == defLabel);
    this->registry.Discard();
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
resourceContainerBase::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
resourceContainerBase::PushLabel(uint8 label) {
    o_assert_dbg(this->valid);
    this->labelStack.Add(label);
}

//------------------------------------------------------------------------------
uint8
resourceContainerBase::PopLabel() {
    o_assert_dbg(this->valid);
    uint8 label = this->labelStack.Back();
    this->labelStack.Erase(this->labelStack.Size() - 1);
    return label;
}

//------------------------------------------------------------------------------
uint8
resourceContainerBase::peekLabel() const {
    o_assert_dbg(this->valid);
    return this->labelStack.Back();
}

//------------------------------------------------------------------------------
Id
resourceContainerBase::Lookup(const Locator& loc) const {
    o_assert_dbg(this->valid);
    return this->registry.Lookup(loc);
}


} // namespace Oryol

