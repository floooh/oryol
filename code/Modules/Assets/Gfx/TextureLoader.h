#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoader
    @ingroup Assets
    @brief standard texture loader for most block-compressed texture file formats
*/
#include "Core/Creator.h"
#include "IO/Stream/Stream.h"
#include "Gfx/Resource/TextureLoaderBase.h"

namespace gliml {
class context;
}

namespace Oryol {

class TextureLoader : public TextureLoaderBase {
    OryolClassDecl(TextureLoader);
    OryolClassCreator(TextureLoaderBase);   // not a bug, creator returns Ptr<TextureLoaderBase>
public:
    /// data is ready, perform loading (can be called from other thread!)
    virtual void Loaded(const URL& url, int32 ioLane, const void* data, int32 numBytes);
    /// loading has failed
    virtual void Failed(const URL& url, int32 ioLane, IOStatus::Code ioStatus);

private:
    /// convert gliml context attrs into a TextureSetup object
    TextureSetup buildSetup(const TextureSetup& blueprint, const gliml::context* ctx, const uint8* data);
    /// return Stream containing loaded data
    Ptr<Stream> buildStream(const void* data, int32 numBytes);
};

} // namespace Oryol