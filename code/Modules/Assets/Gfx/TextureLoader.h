#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoader
    @ingroup Assets
    @brief standard texture loader for most block-compressed texture file formats
*/
#include "Gfx/Resource/TextureLoaderBase.h"

namespace Oryol {

class TextureLoader : public TextureLoaderBase {
    OryolClassDecl(TextureLoader);
public:
    /// test if a resource loading request would be accepted
    virtual bool Accepts(const SetupAndStream<TextureSetup>& input) const;
    /// perform loading
    virtual SetupAndStream<TextureSetup> Load(const SetupAndStream<TextureSetup>& input) const;
};

} // namespace Oryol