#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoader
    @ingroup Assets
    @brief standard texture loader for most block-compressed texture file formats
*/
#include "Gfx/Resource/TextureLoaderBase.h"
#include "IO/Stream/Stream.h"
#include "IO/IOProtocol.h"

namespace gliml {
class context;
}

namespace Oryol {

class TextureLoader : public TextureLoaderBase {
    OryolClassDecl(TextureLoader);
public:
    /// constructor
    TextureLoader(const TextureSetup& setup, int32 ioLane);
    /// destructor
    virtual ~TextureLoader();
    /// start loading, return a resource id
    virtual Id Start();
    /// continue loading, return resource state (Pending, Valid, Failed)
    virtual ResourceState::Code Continue();
    /// cancel the load process
    virtual void Cancel();

private:
    /// convert gliml context attrs into a TextureSetup object
    TextureSetup buildSetup(const TextureSetup& blueprint, const gliml::context* ctx, const uint8* data);
    
    Id resId;
    Ptr<IOProtocol::Request> ioRequest;
};

} // namespace Oryol