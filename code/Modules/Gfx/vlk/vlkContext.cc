//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  vlkContext.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlk_impl.h"
#include "vlkContext.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkContext::~vlkContext() {
    o_assert(nullptr == this->Instance);
    o_assert(nullptr == this->Gpu);
    o_assert(nullptr == this->Device);
    o_assert(nullptr == this->instanceLayers);
    o_assert(nullptr == this->instanceExtensions);
}

//------------------------------------------------------------------------------
void
vlkContext::setup(const GfxSetup& setup) {
    o_assert(nullptr == this->Instance);
    o_assert(nullptr == this->Gpu);
    o_assert(nullptr == this->Device);

    this->setupInstanceLayers();
    this->setupInstanceExtensions();
}

//------------------------------------------------------------------------------
void
vlkContext::discard() {
    o_assert(this->Instance);
    o_assert(this->Gpu);
    o_assert(this->Device);

    this->discardInstanceExtensions();
    this->discardInstanceLayers();
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstanceLayers() {
    o_assert(nullptr == this->instanceLayers);
    o_assert(0 == this->numInstanceLayers);

    // first get number of instance layers, allocate room, and get layer properties
    VkResult err = vkEnumerateInstanceLayerProperties(&this->numInstanceLayers, nullptr);
    o_assert(!err);
    if (this->numInstanceLayers > 0) {
        const int size = this->numInstanceLayers * sizeof(VkLayerProperties);
        this->instanceLayers = (VkLayerProperties*) Memory::Alloc(size);
        err = vkEnumerateInstanceLayerProperties(&this->numInstanceLayers, this->instanceLayers);
    }

    // dump out all instance layers
    Log::Info(">>> Vulkan Instance Layers:\n");
    for (uint32 i = 0; i < this->numInstanceLayers; i++) {
        const VkLayerProperties& cur = this->instanceLayers[i];
        Log::Info("  Name: %s\n    spec version: %x\n    impl version: %x\n    desc: %s\n\n",
            cur.layerName, cur.specVersion, cur.implementationVersion, cur.description);
    }
}

//------------------------------------------------------------------------------
void
vlkContext::discardInstanceLayers() {
    if (this->instanceLayers) {
        Memory::Free(this->instanceLayers);
        this->instanceLayers = nullptr;
    }
    this->numInstanceLayers = 0;
}

//------------------------------------------------------------------------------
void
vlkContext::setupInstanceExtensions() {
    o_assert(nullptr == this->instanceExtensions);
    o_assert(0 == this->numInstanceExtensions);

    VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &this->numInstanceExtensions, nullptr);
    o_assert(!err);
    if (this->numInstanceExtensions > 0) {
        const int size = this->numInstanceExtensions * sizeof(VkExtensionProperties);
        this->instanceExtensions = (VkExtensionProperties*) Memory::Alloc(size);
        err = vkEnumerateInstanceExtensionProperties(nullptr, &this->numInstanceExtensions, this->instanceExtensions);
    }
    Log::Info(">>> Vulkan Instance Extensions:\n");
    for (uint32 i = 0; i < this->numInstanceExtensions; i++) {
        const VkExtensionProperties& cur = this->instanceExtensions[i];
        Log::Info("  %s (ver %x)\n", cur.extensionName, cur.specVersion);
    }
}

//------------------------------------------------------------------------------
void
vlkContext::discardInstanceExtensions() {
    if (this->instanceExtensions) {
        Memory::Free(this->instanceExtensions);
        this->instanceExtensions = nullptr;
    }
    this->numInstanceExtensions = 0;
}

} // namespace _priv
} // namespace Oryol
