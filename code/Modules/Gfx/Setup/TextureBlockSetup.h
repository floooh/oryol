#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureBlockSetup
    @ingroup Gfx
    @brief setup-object for texture blocks
*/
#include "Core/Containers/StaticArray.h"
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {

class TextureBlockSetup {
public:
    /// construct for vertex shader bind stage
    static TextureBlockSetup VS(int32 bindSlot, const Id& shd, uint32 shdSelMask=0);
    /// construct for fragment shader bind stage
    static TextureBlockSetup FS(int32 bindSlot, const Id& shd, uint32 shdSelMask=0);

    /// default constructor
    TextureBlockSetup();

    /// resource locator
    class Locator Locator;
    /// the shader stage to bind to
    ShaderStage::Code BindStage;
    /// the shader stage bind slot to bind to
    int32 BindSlot;
    /// the shader that this texture block is used on
    Id Shader;
    /// the shader selection mask which identifies the sub-shader
    uint32 ShaderSelectionMask;
    /// the texture ids in the block
    StaticArray<Id, GfxConfig::MaxNumTexturesPerStage> Slot;
};

} // namespace Oryol
