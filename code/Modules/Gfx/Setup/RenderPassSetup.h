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
    /// construct from single render target textures, and option depth-stencil texture
    static RenderPassSetup From(Id colorTexture, Id depthStencilTexture=Id::InvalidId());
    /// construct from MRT render target textures, and option depth-stencil texture
    static RenderPassSetup From(std::initializer_list<Id> colorTextures, Id depthStencilTexture=Id::InvalidId());

    /// resource locator
    class Locator Locator = Locator::NonShared();

    /// 1..N color attachments
    struct ColorAttachment {
        Id Texture;
        uint16_t Level = 0;     ///< mipmap-level
        uint16_t Layer = 0;     ///< layer (for 3D- or 2D-array-textures)
        uint16_t Face = 0;      ///< cubemap face
        RenderPassLoadAction::Code LoadAction = RenderPassLoadAction::Clear;
        glm::vec4 ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    };
    StaticArray<ColorAttachment, GfxConfig::MaxNumColorAttachments> ColorAttachments;

    /// optional depth-stencil attachment
    struct DepthStencilAttachment {
        Id Texture;
        RenderPassLoadAction::Code LoadAction = RenderPassLoadAction::Clear;
        float ClearDepth = 1.0f;
        uint8_t ClearStencil = 0;
    };
    struct DepthStencilAttachment DepthStencilAttachment;

    /// what to do in Gfx::EndPass (preserve or drop content, MSAA resolve)
    RenderPassStoreAction::Code StoreAction = RenderPassStoreAction::Store;
};

} // namespace Oryol
