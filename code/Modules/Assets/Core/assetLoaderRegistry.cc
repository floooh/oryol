//------------------------------------------------------------------------------
//  assetLoaderRegistry.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "assetLoaderRegistry.h"

namespace Oryol {

//------------------------------------------------------------------------------
assetLoaderRegistry::assetLoaderRegistry() :
isValid(false),
runLoopId(RunLoop::InvalidId) {
    // empty
}

//------------------------------------------------------------------------------
assetLoaderRegistry::~assetLoaderRegistry() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
assetLoaderRegistry::Setup() {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->runLoopId = Core::PreRunLoop()->Add([this] {
        this->update();
    });
}

//------------------------------------------------------------------------------
void
assetLoaderRegistry::Discard() {
    o_assert_dbg(this->isValid);
    for (const auto& loader : this->loaders) {
        loader->Detached();
    }
    this->loaders.Clear();
    Core::PreRunLoop()->Remove(this->runLoopId);
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
assetLoaderRegistry::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
assetLoaderRegistry::AttachLoader(const Ptr<AssetLoader>& loader) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!this->loaders.FindIndexLinear(loader));
    
    // loaders are attached from front, loaders for the same
    // asset format that are attached later override loaders that
    // are attached earlier
    this->loaders.Insert(0, loader);
}

//------------------------------------------------------------------------------
void
assetLoaderRegistry::DetachLoader(Ptr<AssetLoader> loader) {
    o_assert_dbg(this->isValid);
    
    int32 loaderIndex = this->loaders.FindIndexLinear(loader);
    o_assert_dbg(InvalidIndex != loaderIndex);
    loader->Detached();
    this->loaders.Erase(loaderIndex);
}

//------------------------------------------------------------------------------
FIXME: Load method:

- specialized for asset type by SETUP template param
- get resource Id from Gfx via Gfx::AllocResource
- create IO request
- append to pendingRequests
- then in update: check requests for finished, and call Gfx::SetupResource


} // namespace Oryol
