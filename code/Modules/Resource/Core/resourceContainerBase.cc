//------------------------------------------------------------------------------
//  resourceContainerBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "Core/Assertion.h"
#include "resourceContainerBase.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
resourceContainerBase::resourceContainerBase() :
curLabelCount(0),
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
    this->PushLabel(ResourceLabel::Default);
}

//------------------------------------------------------------------------------
void
resourceContainerBase::discard() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->labelStack.Size() == 1);
    this->PopLabel();
    this->registry.Discard();
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
resourceContainerBase::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
ResourceLabel
resourceContainerBase::PushLabel() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->curLabelCount < ResourceLabel::Default);
    this->labelStack.Add(this->curLabelCount++);
    return this->labelStack.Back();
}

//------------------------------------------------------------------------------
void
resourceContainerBase::PushLabel(ResourceLabel label) {
    o_assert_dbg(this->valid);
    this->labelStack.Add(label);
}

//------------------------------------------------------------------------------
ResourceLabel
resourceContainerBase::PopLabel() {
    o_assert_dbg(this->valid);
    ResourceLabel label = this->labelStack.Back();
    this->labelStack.Erase(this->labelStack.Size() - 1);
    return label;
}

//------------------------------------------------------------------------------
ResourceLabel
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
