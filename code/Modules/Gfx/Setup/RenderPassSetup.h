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
#include <initializer_list>

namespace Oryol {

class RenderPassSetup {
public:
    /// construct from single render target texture
    static RenderPassSetup From(std::initializer_list<Id> colorTextures, Id depthStencilTexture=Id::InvalidId());

    /// resource locator
    class Locator Locator;

    /// 1..N color attachments
    struct ColorAttachment {
        Id Texture;
        uint16_t Level = 0;
        RenderPassLoadAction::Code LoadAction = RenderPassLoadAction::Clear;
        glm::vec4 DefaultClearColor;
    };
    StaticArray<ColorAttachment, GfxConfig::MaxNumColorAttachments> ColorAttachments;

    /// optional depth-stencil attachment
    struct DepthStencilAttachment {
        Id Texture;
        uint16_t Level = 0;
        RenderPassLoadAction::Code LoadAction = RenderPassLoadAction::Clear;
        float DepthClearValue = 1.0f;
        uint8_t StencilClearValue = 0;
    };
    struct DepthStencilAttachment DepthStencilAttachment;

    /// what to do in Gfx::EndPass (preserve or drop content, MSAA resolve)
    RenderPassStoreAction::Code StoreAction = RenderPassStoreAction::Store;
};

} // namespace Oryol
