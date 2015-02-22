#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoaderBase
    @ingroup Gfx
    @brief base class for Gfx texture loaders
*/
#include "Resource/Core/ResourceLoader.h"
#include "Resource/Core/SetupAndStream.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Setup/TextureSetup.h"

namespace Oryol {

class TextureLoaderBase : public ResourceLoader {
    OryolClassDecl(TextureLoaderBase);
public:
    /// indicate supported resource types
    bool Supports(GfxResourceType::Code resType) const;
    /// test if a resource loading request would be accepted
    virtual bool Accepts(const SetupAndStream<TextureSetup>& input) const;
    /// perform loading
    virtual SetupAndStream<TextureSetup> Load(const SetupAndStream<TextureSetup>& input) const;
};

} // namespace Oryol

