//------------------------------------------------------------------------------
//  assetCreator_Gfx.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "assetCreator.h"
#include "Gfx/Gfx.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
template<class SETUP> Id
assetCreator::Create(const SETUP& setup, assetRegistry* registry) {
    this->regFuncsOnce(Gfx::PushResourceLabel, Gfx::PopResourceLabel, Gfx::DiscardResources);
    Id resId = Gfx::CreateResource(setup);
    registry->Add(setup.Locator, resId);
    return resId;
}

//------------------------------------------------------------------------------
template<class SETUP> Id
assetCreator::Create(const SETUP& setup, const Ptr<Stream>& data, assetRegistry* registry) {
    this->regFuncsOnce(Gfx::PushResourceLabel, Gfx::PopResourceLabel, Gfx::DiscardResources);
    Id resId = Gfx::CreateResource(setup, data);
    registry->Add(setup.Locator, resId);
    return resId;
}

//------------------------------------------------------------------------------
template<class SETUP> Id
assetCreator::Alloc(const SETUP& setup, Id placeholder, assetRegistry* registry) {
    this->regFuncsOnce(Gfx::PushResourceLabel, Gfx::PopResourceLabel, Gfx::DiscardResources);
    Id resId = Gfx::AllocResource(setup, placeholder);
    registry->Add(setup.Locator, resId);
    return resId;
}

//------------------------------------------------------------------------------
template<class SETUP> void
assetCreator::Init(const Id& id, const SETUP& setup, const Ptr<Stream>& data) {
    Gfx::InitResource(id, setup, data);
}

} // namespace _priv
} // namespace Oryol
