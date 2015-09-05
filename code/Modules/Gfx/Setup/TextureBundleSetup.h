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
    /// construct from shader
    static TextureBundleSetup FromShader(const Id& shd, uint32 shdSelMask=0);

    /// default constructor
    TextureBundleSetup();

    /// resource locator
    class Locator Locator;
    /// the shader that this texture bundle is used on
    Id Shader;
    /// the shader selection mask which identifies the sub-shader
    uint32 ShaderSelectionMask;
    /// vertex shader texture slots
    StaticArray<Id, GfxConfig::MaxNumVSTextures> VS;
    /// fragment shader texture slots
    StaticArray<Id, GfxConfig::MaxNumFSTextures> FS;
};

} // namespace Oryol