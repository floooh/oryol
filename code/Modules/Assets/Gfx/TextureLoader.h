#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoader
    @ingroup Assets
    @brief standard texture loader for most block-compressed texture file formats
*/
#include "Core/Creator.h"
#include "Gfx/Resource/TextureLoaderBase.h"

namespace Oryol {

class TextureLoader : public TextureLoaderBase {
    OryolClassDecl(TextureLoader);
    OryolClassCreator(TextureLoaderBase);   // not a bug, creator returns Ptr<TextureLoaderBase>
public:
    /// data is ready, perform loading (can be called from other thread!)
    virtual void Loaded(const URL& url, int32 ioLane, const void* data, int32 numBytes);
    /// loading has failed
    virtual void Failed(const URL& url, IOStatus::Code ioStatus);
};

} // namespace Oryol