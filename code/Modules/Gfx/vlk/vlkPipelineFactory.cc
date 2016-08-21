//------------------------------------------------------------------------------
//  vlkPipelineFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkPipelineFactory.h"
#include "vlkTypes.h"
#include "Core/Assertion.h"
#include "Gfx/Resource/resource.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/Core/displayMgr.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
vlkPipelineFactory::Setup(const gfxPointers& ptrs) {
    pipelineFactoryBase::Setup(ptrs);
    VkDevice device = this->pointers.displayMgr->vlkContext.Device;
    o_assert_dbg(device);

    // create a pipeline cache object
    VkPipelineCacheCreateInfo info = { VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
    VkResult err = vkCreatePipelineCache(device, &info, nullptr, &this->pipelineCache);
    o_assert(!err && this->pipelineCache);
}

//------------------------------------------------------------------------------
void
vlkPipelineFactory::Discard() {
    VkDevice device = this->pointers.displayMgr->vlkContext.Device;
    o_assert_dbg(device);
    if (this->pipelineCache) {
        vkDestroyPipelineCache(device, this->pipelineCache, nullptr);
    }
    pipelineFactoryBase::Discard();
}

//------------------------------------------------------------------------------
static int
describeVertexBinding(pipeline& pip, VkVertexInputBindingDescription* vibs) {
    int vlkBindIndex = 0;
    for (int mshSlotIndex = 0; mshSlotIndex < GfxConfig::MaxNumInputMeshes; mshSlotIndex++) {
        const VertexLayout& layout = pip.Setup.Layouts[mshSlotIndex];
        if (!layout.Empty()) {
            auto& curVib = vibs[mshSlotIndex];
            curVib.binding = vlkBindIndex++;
            curVib.stride = layout.ByteSize();
            if (layout.StepFunction == VertexStepFunction::PerVertex) {
                curVib.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }
            else {
                curVib.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            }
        }
    }
    return vlkBindIndex;
}

//------------------------------------------------------------------------------
static int
describeVertexAttrs(pipeline& pip, VkVertexInputAttributeDescription* vias) {
    int vlkBindIndex = 0;
    int vlkCompIndex = 0;
    for (int mshSlotIndex = 0; mshSlotIndex < GfxConfig::MaxNumInputMeshes; mshSlotIndex++) {
        const VertexLayout& layout = pip.Setup.Layouts[mshSlotIndex];
        for (int compIndex = 0; compIndex < layout.NumComponents(); compIndex++, vlkCompIndex++) {
            const auto& comp = layout.ComponentAt(compIndex);
            o_assert_dbg(vlkCompIndex < VertexAttr::NumVertexAttrs);
            auto& curVia = vias[vlkCompIndex];
            curVia.location = comp.Attr;
            curVia.binding = vlkBindIndex;
            curVia.format = vlkTypes::asVertexFormat(comp.Format);
            curVia.offset = layout.ComponentByteOffset(compIndex);
        }
        vlkBindIndex++;
    }
    return vlkCompIndex;
}

//------------------------------------------------------------------------------
static VkStencilOpState
asStencilOpState(const DepthStencilState& depthStencilState, const StencilState& stencilState) {
    VkStencilOpState out = { };
    out.failOp = vlkTypes::asStencilOp(stencilState.FailOp);
    out.passOp = vlkTypes::asStencilOp(stencilState.PassOp);
    out.depthFailOp = vlkTypes::asStencilOp(stencilState.DepthFailOp);
    out.compareOp = vlkTypes::asCompareOp(stencilState.CmpFunc);
    out.compareMask = depthStencilState.StencilReadMask;
    out.writeMask = depthStencilState.StencilWriteMask;
    out.reference = depthStencilState.StencilRef;
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkPipelineFactory::SetupResource(pipeline& pip) {
    o_assert_dbg(!pip.vkPipeline);    
    pipelineFactoryBase::SetupResource(pip);
    o_assert_dbg(pip.shd);
    VkDevice device = this->pointers.displayMgr->vlkContext.Device;
    o_assert_dbg(device);

    static const VkDynamicState dynStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynStateInfo.dynamicStateCount = 2;
    dynStateInfo.pDynamicStates = dynStates;

    VkPipelineShaderStageCreateInfo stageInfo[2] = {
        { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
        { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO }
    };
    stageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stageInfo[0].module = pip.shd->vsModule;
    stageInfo[0].pName = "main";
    stageInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stageInfo[1].module = pip.shd->fsModule;
    stageInfo[1].pName = "main";

    VkVertexInputBindingDescription vibDesc[VertexAttr::NumVertexAttrs];
    VkVertexInputAttributeDescription viaDesc[VertexAttr::NumVertexAttrs];
    int numBindings = describeVertexBinding(pip, vibDesc);
    int numAttrs = describeVertexAttrs(pip, viaDesc);
    VkPipelineVertexInputStateCreateInfo visInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    visInfo.vertexBindingDescriptionCount = numBindings;
    visInfo.pVertexBindingDescriptions = vibDesc;
    visInfo.vertexAttributeDescriptionCount = numAttrs;
    visInfo.pVertexAttributeDescriptions = viaDesc;

    VkPipelineInputAssemblyStateCreateInfo piasInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    piasInfo.primitiveRestartEnable = VK_FALSE;
    piasInfo.topology = vlkTypes::asPrimitiveTopology(pip.Setup.PrimType);

    VkPipelineViewportStateCreateInfo pvsInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    pvsInfo.viewportCount = 1;
    pvsInfo.scissorCount = 1;

    const auto& rs = pip.Setup.RasterizerState;
    VkPipelineRasterizationStateCreateInfo prsInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    prsInfo.depthClampEnable = VK_FALSE;
    prsInfo.rasterizerDiscardEnable = VK_FALSE;
    prsInfo.polygonMode = VK_POLYGON_MODE_FILL;
    prsInfo.cullMode = vlkTypes::asCullMode(rs.CullFaceEnabled, rs.CullFace);
    prsInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;    // OpenGL convention
    prsInfo.depthBiasEnable = VK_FALSE;
    prsInfo.depthBiasConstantFactor = 0.0f;
    prsInfo.depthBiasClamp = 0.0f;
    prsInfo.depthBiasSlopeFactor = 0.0f;
    prsInfo.lineWidth = 0.0f;

    VkPipelineMultisampleStateCreateInfo pmsInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    pmsInfo.rasterizationSamples = (VkSampleCountFlagBits)rs.SampleCount;
    pmsInfo.sampleShadingEnable = VK_FALSE;
    pmsInfo.alphaToCoverageEnable = VK_FALSE;
    pmsInfo.alphaToOneEnable = VK_FALSE;

    const auto& dss = pip.Setup.DepthStencilState;
    VkPipelineDepthStencilStateCreateInfo pdsInfo = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
    pdsInfo.depthTestEnable = (dss.DepthCmpFunc != CompareFunc::Never);
    pdsInfo.depthWriteEnable = dss.DepthWriteEnabled;
    pdsInfo.depthCompareOp = vlkTypes::asCompareOp(dss.DepthCmpFunc);
    pdsInfo.depthBoundsTestEnable = VK_FALSE;
    pdsInfo.stencilTestEnable = dss.StencilEnabled;
    pdsInfo.front = asStencilOpState(dss, dss.StencilFront);
    pdsInfo.back = asStencilOpState(dss, dss.StencilBack);
    pdsInfo.minDepthBounds = 0.0f;
    pdsInfo.maxDepthBounds = 0.0f;

    VkPipelineColorBlendStateCreateInfo pcbInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
FIXME

    VkGraphicsPipelineCreateInfo pipInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipInfo.stageCount = 2;
    pipInfo.pStages = stageInfo;
    pipInfo.pVertexInputState = &visInfo;
    pipInfo.pInputAssemblyState = &piasInfo;
    pipInfo.pTessellationState = nullptr;
    pipInfo.pViewportState = &pvsInfo;
    pipInfo.pDynamicState = &dynStateInfo;
    pipInfo.pRasterizationState = &prsInfo;
    pipInfo.pMultisampleState = &pmsInfo;
    pipInfo.pDepthStencilState = &pdsInfo;

    VkResult err = vkCreateGraphicsPipelines(
        device, 
        this->pipelineCache,
        1,
        &pipInfo,
        nullptr,
        &pip.vkPipeline);
    o_assert(!err && pip.vkPipeline);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
vlkPipelineFactory::DestroyResource(pipeline& pip) {
    o_assert_dbg(this->isValid);
    if (pip.vkPipeline) {
        vlkResAllocator& resAllocator = this->pointers.displayMgr->vlkContext.ResAllocator;
        const uint64 frameIndex = this->pointers.displayMgr->vlkContext.CurFrameIndex;
        resAllocator.releasePipeline(frameIndex, pip.vkPipeline);
    }
    pipelineFactoryBase::DestroyResource(pip);
}

} // namespace _priv
} // namespace Oryol
