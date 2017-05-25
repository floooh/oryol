//------------------------------------------------------------------------------
//  ResourceContainerBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Core.h"
#include "Core/Assertion.h"
#include "ResourceContainerBase.h"

namespace Oryol {
    
//------------------------------------------------------------------------------
ResourceContainerBase::~ResourceContainerBase() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
ResourceContainerBase::Setup(int labelStackCapacity, int registryCapacity) {
    o_assert_dbg(!this->valid);
    this->labelStack.Reserve(labelStackCapacity);
    this->registry.Setup(registryCapacity);
    this->valid = true;
    this->PushLabel(ResourceLabel::Default);
}

//------------------------------------------------------------------------------
void
ResourceContainerBase::Discard() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->labelStack.Size() == 1);
    this->PopLabel();
    this->registry.Discard();
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
ResourceContainerBase::IsValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
ResourceLabel
ResourceContainerBase::PushLabel() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->curLabelCount < ResourceLabel::Default);
    this->labelStack.Add(this->curLabelCount++);
    return this->labelStack.Back();
}

//------------------------------------------------------------------------------
void
ResourceContainerBase::PushLabel(ResourceLabel label) {
    o_assert_dbg(this->valid);
    this->labelStack.Add(label);
}

//------------------------------------------------------------------------------
ResourceLabel
ResourceContainerBase::PopLabel() {
    o_assert_dbg(this->valid);
    ResourceLabel label = this->labelStack.Back();
    this->labelStack.Erase(this->labelStack.Size() - 1);
    return label;
}

//------------------------------------------------------------------------------
ResourceLabel
ResourceContainerBase::PeekLabel() const {
    o_assert_dbg(this->valid);
    return this->labelStack.Back();
}

//------------------------------------------------------------------------------
Id
ResourceContainerBase::Lookup(const Locator& loc) const {
    o_assert_dbg(this->valid);
    return this->registry.Lookup(loc);
}

} // namespace Oryol
