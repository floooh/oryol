#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoader
    @ingroup Assets
    @brief standard texture loader for most block-compressed texture file formats
*/
#include "Gfx/TextureLoaderBase.h"
#include "IO/private/ioRequests.h"

namespace gliml {
class context;
}

namespace Oryol {

class TextureLoader : public TextureLoaderBase {
    OryolClassDecl(TextureLoader);
public:
    /// constructor without success-callback
    TextureLoader(const TextureSetup& setup);
    /// constructor with success callback
    TextureLoader(const TextureSetup& setup, LoadedFunc onLoaded);
    /// destructor
    ~TextureLoader();
    /// start loading, return a resource id
    virtual Id Start() override;
    /// continue loading, return resource state (Pending, Valid, Failed)
    virtual ResourceState::Code Continue() override;
    /// cancel the load process
    virtual void Cancel() override;

private:
    /// convert gliml context attrs into a TextureSetup object
    TextureSetup buildSetup(const TextureSetup& blueprint, const gliml::context* ctx, const uint8_t* data);
    
    Id resId;
    Ptr<IORead> ioRequest;
};

} // namespace Oryol