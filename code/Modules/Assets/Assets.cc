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

//------------------------------------------------------------------------------
const class AssetsSetup&
Assets::AssetsSetup() {
    o_assert_dbg(IsValid());
    return state->setup;
}

//------------------------------------------------------------------------------
void
Assets::AttachLoader(const Ptr<AssetLoader>& loader) {
    o_assert_dbg(IsValid());
    state->loaderRegistry.AttachLoader(loader);
}

//------------------------------------------------------------------------------
void
Assets::DetachLoader(const Ptr<AssetLoader>& loader) {
    o_assert_dbg(IsValid());
    state->loaderRegistry.DetachLoader(loader);
}

//------------------------------------------------------------------------------
AssetId
Assets::Lookup(const Locator& loc) {
    o_assert_dbg(IsValid());
    return AssetId(state->registry.Lookup(loc));
}

//------------------------------------------------------------------------------
AssetId
Assets::Register(const Locator& loc, Id resId, DiscardFunc discardFunc) {
    o_assert_dbg(IsValid());
    o_assert_dbg(!state->registry.Contains(id));
    o_assert_dbg(!state->registry.Lookup(loc).IsValid());
    state->registry.Add(loc, resId, discardFunc);
    return AssetId(resId);
}

} // namespace Oryol