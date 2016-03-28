#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkShader
    @ingroup _priv
    @brief Vulkan implementation of class shader
*/
#include "Gfx/Resource/shaderBase.h"
#include "Gfx/vlk/vlk_impl.h"

namespace Oryol {
namespace _priv {

class vlkShader : public shaderBase {
public:
    /// destructor
    ~vlkShader();
    /// clear the object
    void Clear();

    VkShaderModule vsModule = nullptr;
    VkShaderModule fsModule = nullptr;
};

} // namespace _priv
} // namespace Oryol
