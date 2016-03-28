//------------------------------------------------------------------------------
//  vlkShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkShaderFactory.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Core/displayMgr.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkShaderFactory::~vlkShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
vlkShaderFactory::Setup(const gfxPointers& ptr) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptr;
}

//------------------------------------------------------------------------------
void
vlkShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
vlkShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!shd.vsModule);
    o_assert_dbg(!shd.fsModule);

    VkDevice device = this->pointers.displayMgr->vlkContext.Device;
    o_assert_dbg(device);
    const void* ptr = nullptr;
    uint32 size = 0;

    shd.Setup.VertexShaderByteCode(ShaderLang::SPIRV, ptr, size);
    o_assert_dbg(ptr && (size > 0));
    VkShaderModuleCreateInfo shdInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    shdInfo.codeSize = size;
    shdInfo.pCode = (const uint32_t*) ptr;
    VkResult err = vkCreateShaderModule(device, &shdInfo, nullptr, &shd.vsModule);
    o_assert(!err && shd.vsModule);

    shd.Setup.FragmentShaderByteCode(ShaderLang::SPIRV, ptr, size);
    o_assert_dbg(ptr && (size > 0));
    shdInfo.codeSize = size;
    shdInfo.pCode = (const uint32_t*) ptr;
    err = vkCreateShaderModule(device, &shdInfo, nullptr, &shd.fsModule);
    o_assert(!err && shd.fsModule);

    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
void
vlkShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);
    // NOTE: don't need to defer-release shader modules, since they are
    // already no longer needed after creating pipeline objects
    VkDevice device = this->pointers.displayMgr->vlkContext.Device;
    o_assert_dbg(device);
    if (shd.vsModule) {
        vkDestroyShaderModule(device, shd.vsModule, nullptr);
    }
    if (shd.fsModule) {
        vkDestroyShaderModule(device, shd.fsModule, nullptr);
    }
    shd.Clear();
}

} // namespace _priv
} // namespace Oryol
