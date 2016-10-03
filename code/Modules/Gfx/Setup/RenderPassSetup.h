#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::RenderPassSetup
    @ingroup Gfx
    @brief setup attributes for RenderPass resource
*/
#include "Core/Containers/StaticArray.h"
#include "Gfx/Core/GfxConfig.h"
#include "Gfx/Core/Enums.h"
#include "Resource/Id.h"
#include "Resource/Locator.h"
#include "glm/vec4.hpp"

namespace Oryol {

class RenderPassSetup {
public:
    /// construct for single color attachment (no depth-stencil-buffer)
    static RenderPassSetup FromColorTexture(Id colorTex);
    /// construct for single color attachment and depth-stencil-buffer
    static RenderPassSetup FromColorAndDepthStencilTextures(Id colorTex, Id depthStencilTex);

    /// resource locator
    class Locator Locator;

    /// 1..N color attachments
    struct ColorAttachment {
        Id Texture;
        uint16_t Level = 0;
        RenderPassLoadAction::Code LoadAction = RenderPassLoadAction::Clear;
        RenderPassStoreAction::Code StoreAction = RenderPassStoreAction::Store;
        glm::vec4 DefaultClearColor;
    };
    StaticArray<ColorAttachment, GfxConfig::MaxNumColorAttachments> ColorAttachments;

    /// optional depth-stencil attachment
    struct DepthStencilAttachment {
        Id Texture;
        uint16_t Level = 0;
        RenderPassLoadAction::Code LoadAction = RenderPassLoadAction::Clear;
        RenderPassStoreAction::Code StoreAction = RenderPassStoreAction::Store;
        float DepthClearValue = 1.0f;
        uint8_t StencilClearValue = 0;
    };
    struct DepthStencilAttachment DepthStencilAttachment;
};

} // namespace Oryol
