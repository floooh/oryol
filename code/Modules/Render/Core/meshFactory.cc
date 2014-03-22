//------------------------------------------------------------------------------
//  meshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "meshFactory.h"
#include "Render/base/meshLoaderBase.h"

namespace Oryol {
namespace Render {

using namespace IO;
using namespace Core;

//------------------------------------------------------------------------------
uint16
meshFactory::GetResourceType() const {
    return ResourceType::Mesh;
}

//------------------------------------------------------------------------------
void
meshFactory::AttachLoader(const Ptr<meshLoaderBase>& loader) {
    o_assert(InvalidIndex == this->loaders.FindIndexLinear(loader));
    loader->onAttachToFactory(this);
    this->loaders.AddBack(loader);
}

//------------------------------------------------------------------------------
bool
meshFactory::NeedsSetupResource(const mesh& mesh) const {
    o_assert(mesh.GetState() == Resource::State::Pending);
    const Ptr<IOProtocol::Request>& ioRequest = mesh.GetIORequest();
    if (ioRequest.isValid()) {
        return ioRequest->Handled();
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
meshFactory::DestroyResource(mesh& mesh) {
    // We need to hook in here and check whether there's an asynchronous
    // IORequest in flight. If yes, cancel it so that it doesn't load data
    // which actually isn't needed anymore
    const Ptr<IOProtocol::Request>& ioRequest = mesh.GetIORequest();
    if (ioRequest.isValid()) {
        ioRequest->SetCancelled();
    }
    glMeshFactory::DestroyResource(mesh);
}


} // namespace Render
} // namespace Oryol