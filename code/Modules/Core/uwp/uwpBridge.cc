//------------------------------------------------------------------------------
//  uwpBridge.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "uwpBridge.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

uwpBridge* uwpBridge::self = nullptr;

//------------------------------------------------------------------------------
uwpBridge::uwpBridge() :
app(nullptr) {
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
uwpBridge::~uwpBridge() {
    o_assert(nullptr == this->app);
    o_assert(nullptr != self);
    self = nullptr;
}

//------------------------------------------------------------------------------
uwpBridge*
uwpBridge::ptr() {
    o_assert_dbg(nullptr != self);
    return self;
}

//------------------------------------------------------------------------------
void
uwpBridge::setup(App* app_) {
    o_assert(nullptr == this->app);
    o_assert(nullptr != app_);
    this->app = app_;
    Log::Info("uwpBridge::setup() called!\n");
}

//------------------------------------------------------------------------------
void
uwpBridge::discard() {
    o_assert(nullptr != this->app);
    Log::Info("uwpBridge::discard() called!\n");
    this->app = nullptr;
}

//------------------------------------------------------------------------------
void
uwpBridge::startMainLoop() {
    Log::Info("uwpBridge::startMainLoop() called!\n");
    // FIXME FIXME FIXME
}

} // namespace _priv
} // namespace Oryol
