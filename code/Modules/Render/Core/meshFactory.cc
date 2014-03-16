//------------------------------------------------------------------------------
//  meshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "meshFactory.h"

namespace Oryol {
namespace Render {

using namespace Core;
    
//------------------------------------------------------------------------------
void
meshFactory::AttachLoader(const Ptr<meshLoaderBase>& loader) {
    o_assert(InvalidIndex == this->loaders.FindIndexLinear(loader));
    loader->onAttachToFactory(this);
    this->loaders.AddBack(loader);
}

} // namespace Render
} // namespace Oryol