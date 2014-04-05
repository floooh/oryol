//------------------------------------------------------------------------------
//  textureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "textureFactory.h"
#include "Render/base/textureLoaderBase.h"

namespace Oryol {
namespace Render {
    
using namespace Core;

//------------------------------------------------------------------------------
uint16
textureFactory::GetResourceType() const {
    return ResourceType::Texture;
}
    
//------------------------------------------------------------------------------
/**
 FIXME: this could probably go into loaderFactory?
*/
void
textureFactory::AttachLoader(const Ptr<textureLoaderBase>& loader) {
    o_assert(InvalidIndex == this->loaders.FindIndexLinear(loader));
    loader->onAttachToFactory(this);
    this->loaders.AddBack(loader);
}
    
//------------------------------------------------------------------------------
/**
 FIXME: this could probably go into loaderFactory?
 */
bool
textureFactory::NeedsSetupResource(const texture& tex) const {
    o_assert(tex.GetState() == Resource::State::Pending);
    const Ptr<IO::IOProtocol::Request>& ioRequest = tex.GetIORequest();
    if (ioRequest.isValid()) {
        return ioRequest->Handled();
    }
    else {
        return false;
    }
}
    
//------------------------------------------------------------------------------
/**
 FIXME: this could probably go into loaderFactory?
 */
void
textureFactory::DestroyResource(texture& tex) {
    // We need to hook in here and check whether there's an asynchronous
    // IORequest in flight. If yes, cancel it so that it doesn't load data
    // which actually isn't needed anymore
    const Ptr<IO::IOProtocol::Request>& ioRequest = tex.GetIORequest();
    if (ioRequest.isValid()) {
        ioRequest->SetCancelled();
    }
    glTextureFactory::DestroyResource(tex);
}
    
} // namespace Render
} // namespace Oryol
