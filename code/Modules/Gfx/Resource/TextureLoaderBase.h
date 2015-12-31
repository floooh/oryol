#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoaderBase
    @ingroup Gfx
    @brief base class for Gfx texture loaders
*/
#include "Resource/Core/ResourceLoader.h"
#include "Gfx/Setup/TextureSetup.h"

namespace Oryol {

class TextureLoaderBase : public ResourceLoader {
    OryolClassDecl(TextureLoaderBase);
public:
    /// constructor
    TextureLoaderBase(const TextureSetup& setup);
    /// return resource locator
    virtual class Locator Locator() const override;

protected:
    TextureSetup setup;
};

} // namespace Oryol

