#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoaderBase
    @ingroup Gfx
    @brief base class for Gfx texture loaders
*/
#include "Resource/ResourceLoader.h"
#include "Gfx/GfxTypes.h"
#include <functional>

namespace Oryol {

class TextureLoaderBase : public ResourceLoader {
    OryolClassDecl(TextureLoaderBase);
public:
    /// optional callback when loading has succeeded
    typedef std::function<void(TextureSetup&)> LoadedFunc;

    /// constructor
    TextureLoaderBase(const TextureSetup& setup);
    /// constructor
    TextureLoaderBase(const TextureSetup& setup, LoadedFunc onLoaded);
    /// return resource locator
    virtual class Locator Locator() const override;

protected:
    TextureSetup setup;
    std::function<void(TextureSetup&)> onLoaded;
};

} // namespace Oryol

