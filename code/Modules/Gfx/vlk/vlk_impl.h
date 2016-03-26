#pragma once
//------------------------------------------------------------------------------
/**
    @file vlk_impl.h
    @brief includes the Vulkan headers
*/
#include <vulkan/vulkan.h>

// extension function pointers
#define INST_FUNC_PTR(inst, func) \
{ \
    fp##func = (PFN_vk##func)vkGetInstanceProcAddr(inst, "vk" #func);\
    o_assert(fp##func);\
}
