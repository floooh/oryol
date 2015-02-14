//------------------------------------------------------------------------------
//  Assets.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Assets.h"

namespace Oryol {

using namespace _priv;

Assets::_state* Assets::state = nullptr;

//------------------------------------------------------------------------------
void
Assets::Setup(const class AssetsSetup& setup) {
    o_assert_dbg(!IsValid());
    state = new _state();
    state->setup = setup;
    state->registry.Setup(setup.RegistryCapacity);
}

//------------------------------------------------------------------------------
void
Assets::Discard() {
    o_assert_dbg(IsValid());
    state->registry.Discard();
    delete state;
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
Assets::IsValid() {
    return nullptr != state;
}


FIXME:
- Load must first check the registry, then call assetLoaderRegistry

} // namespace Oryol