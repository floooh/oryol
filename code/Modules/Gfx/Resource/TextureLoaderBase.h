#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoaderBase
    @ingroup Gfx
    @brief base class for Gfx texture loaders
*/
#include "IO/Core/IOLoader.h"
#include "Gfx/Setup/TextureSetup.h"

namespace Oryol {

class TextureLoaderBase : public IOLoader {
    OryolClassDecl(TextureLoaderBase);
public:
    /// prepare texture loader (called on main thread)
    virtual void Prepare(const Id& id, const TextureSetup& setup);

protected:
    TextureSetup setup;
    Id id;
};

} // namespace Oryol

