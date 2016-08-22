#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkTypes
    @brief Vulkan type conversions
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"
#include "vlk_impl.h"

namespace Oryol {
namespace _priv {

class vlkTypes {
public:
    /// convert VkPhysicalDeviceType to static string
    static const char* physicalDeviceTypeAsString(VkPhysicalDeviceType t);
    /// convert PixelFormat to a renderable Vulkan format
    static VkFormat asRenderTargetFormat(PixelFormat::Code pixelFormat);
    /// convert VertexFormat to VkFormat
    static VkFormat asVertexFormat(VertexFormat::Code vertexFormat);
    /// convert PrimitiveType to VkPrimitiveTopology
    static VkPrimitiveTopology asPrimitiveTopology(PrimitiveType::Code primType);
    /// convert Oryol cull state into VkCullModeFlags
    static VkCullModeFlags asCullMode(bool cullEnabled, Face::Code face);
    /// convert CompareFunc to VkCompareOp
    static VkCompareOp asCompareOp(CompareFunc::Code cmp);
    /// convert StencilOp to VkStencilOp
    static VkStencilOp asStencilOp(StencilOp::Code op);
    /// convert BlendFactor to VkBlendFactor
    static VkBlendFactor asBlendFactor(BlendFactor::Code bf);
    /// convert BlendOperation to VkBlendOp
    static VkBlendOp asBlendOp(BlendOperation::Code bo);
    /// convert color write mask
    static VkColorComponentFlags asColorWriteMask(PixelChannel::Mask mask);
};

} // namespace _priv
} // namespace Oryol
