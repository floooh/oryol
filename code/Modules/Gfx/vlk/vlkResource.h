#pragma once
//------------------------------------------------------------------------------
#include "Gfx/Resource/resourceBase.h"
#include "Gfx/vlk/vlk_impl.h"
#include "Gfx/vlk/vlkConfig.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkMesh
    @ingroup _priv
    @brief Vulkan implementation of mesh
*/
class vlkMesh : public meshBase {
public:
    /// destructor
    ~vlkMesh();

    /// clear the object (called from meshFactory::DestroyResource())
    void Clear();

    static const int32 NumSlots = vlkConfig::NumFrames;
    struct buffer {
        uint64 frameIndex = InvalidIndex;
        uint8 numSlots = 1;
        uint8 activeSlot = 0;
        struct slot {
            VkBuffer renderBuffer = nullptr;
            VkBuffer stagingBuffer = nullptr;
            VkDeviceMemory renderBufferMemory = nullptr;
            VkDeviceMemory stagingBufferMemory = nullptr;
        };
        StaticArray<slot, vlkConfig::NumFrames> slots;
    };
    static const int vb = 0;
    static const int ib = 1;
    StaticArray<buffer, 2> buffers;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkShader
    @ingroup _priv
    @brief Vulkan implementation of class shader
*/
class vlkShader : public shaderBase {
public:
    /// destructor
    ~vlkShader();
    /// clear the object
    void Clear();

    VkShaderModule vsModule = nullptr;
    VkShaderModule fsModule = nullptr;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkPipeline
    @ingroup _priv
    @brief Vulkan implementation of pipeline
*/
class vlkPipeline : public pipelineBase {
    // FIXME
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkTexture
    @ingroup _priv
    @brief Vulkan implementation of class texture
*/
class vlkTexture : public textureBase {
    // FIXME
};

} // namespace _priv
} // namespace Oryol

