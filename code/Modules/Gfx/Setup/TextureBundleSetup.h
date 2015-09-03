#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureBundleSetup
    @ingroup Gfx
    @brief setup-object for texture bundles
*/
#include "Core/Containers/StaticArray.h"
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Gfx/Core/GfxConfig.h"

namespace Oryol {

class TextureBundleSetup {
public:
    /// default constructor
    TextureBundleSetup();
    /// construct with locator
    TextureBundleSetup(const Locator& loc);

    /// resource locator
    class Locator Locator;

    /// vertex shader texture slots
    StaticArray<Id, GfxConfig::MaxNumVSTextures> VS;
    /// fragment shader texture slots
    StaticArray<Id, GfxConfig::MaxNumFSTextures> FS;
};

} // namespace Oryol