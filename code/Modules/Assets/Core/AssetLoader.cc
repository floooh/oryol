//------------------------------------------------------------------------------
//  AssetLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "AssetLoader.h"

namespace Oryol {

OryolClassImpl(AssetLoader);

//------------------------------------------------------------------------------
void
AssetLoader::Attached() {
    // empty
}

//------------------------------------------------------------------------------
void
AssetLoader::Detached() {
    // empty
}

//------------------------------------------------------------------------------
bool
AssetLoader::TryLoad(const Id& /*id*/, const Ptr<Stream>& /*data*/) {
    return false;
}

} // namespace Oryol