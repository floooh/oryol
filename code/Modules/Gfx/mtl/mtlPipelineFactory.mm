//------------------------------------------------------------------------------
//  mtlPipelineFactory.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlPipelineFactory.h"
#include "mtl_impl.h"
#include "mtlTypes.h"
#include "Gfx/Resource/pipeline.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
ResourceState::Code
mtlPipelineFactory::SetupResource(pipeline& pip) {
    o_assert_dbg(this->isValid);

    pipelineFactoryBase::SetupResource(pip);
    o_assert_dbg(pip.shd);
    this->createRPS(pip);
    this->createDSS(pip);
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlPipelineFactory::DestroyResource(pipeline& pip) {
    o_assert_dbg(this->isValid);
    if (nil != pip.mtlRenderPipelineState) {
        this->pointers.renderer->releaseDeferred(pip.mtlRenderPipelineState);
    }
    if (nil != pip.mtlDepthStencilState) {
        this->pointers.renderer->releaseDeferred(pip.mtlDepthStencilState);
    }
    pipelineFactoryBase::DestroyResource(pip);
}

//------------------------------------------------------------------------------
void
mtlPipelineFactory::createRPS(pipeline& pip) {
    o_assert_dbg(nil == pip.mtlRenderPipelineState);

    // create vertex-descriptor object
    MTLVertexDescriptor* vtxDesc = [MTLVertexDescriptor vertexDescriptor];
    for (int meshSlotIndex = 0; meshSlotIndex < GfxConfig::MaxNumInputMeshes; meshSlotIndex++) {
        // NOTE: vertex buffers are located after constant buffers
        const int vbSlotIndex = meshSlotIndex + GfxConfig::MaxNumUniformBlocksPerStage;
        const VertexLayout& layout = pip.Setup.Layouts[meshSlotIndex];
        for (int compIndex = 0; compIndex < layout.NumComponents(); compIndex++) {
            const auto& comp = layout.ComponentAt(compIndex);
            vtxDesc.attributes[comp.Attr].format = mtlTypes::asVertexFormat(comp.Format);
            vtxDesc.attributes[comp.Attr].bufferIndex = vbSlotIndex;
            vtxDesc.attributes[comp.Attr].offset = layout.ComponentByteOffset(compIndex);
        }
        vtxDesc.layouts[vbSlotIndex].stride = layout.ByteSize();
        vtxDesc.layouts[vbSlotIndex].stepFunction = mtlTypes::asVertexStepFunc(layout.StepFunction);
        vtxDesc.layouts[vbSlotIndex].stepRate = layout.StepRate;
    }

    // create renderpipeline-state
    const BlendState& blendState = pip.Setup.BlendState;
    MTLRenderPipelineDescriptor* rpDesc = [[MTLRenderPipelineDescriptor alloc] init];
    rpDesc.colorAttachments[0].pixelFormat = mtlTypes::asRenderTargetColorFormat(blendState.ColorFormat);
    rpDesc.colorAttachments[0].writeMask = mtlTypes::asColorWriteMask(blendState.ColorWriteMask);
    rpDesc.colorAttachments[0].blendingEnabled = blendState.BlendEnabled;
    rpDesc.colorAttachments[0].alphaBlendOperation = mtlTypes::asBlendOp(blendState.OpAlpha);
    rpDesc.colorAttachments[0].rgbBlendOperation = mtlTypes::asBlendOp(blendState.OpRGB);
    rpDesc.colorAttachments[0].destinationAlphaBlendFactor = mtlTypes::asBlendFactor(blendState.DstFactorAlpha);
    rpDesc.colorAttachments[0].destinationRGBBlendFactor = mtlTypes::asBlendFactor(blendState.DstFactorRGB);
    rpDesc.colorAttachments[0].sourceAlphaBlendFactor = mtlTypes::asBlendFactor(blendState.SrcFactorAlpha);
    rpDesc.colorAttachments[0].sourceRGBBlendFactor = mtlTypes::asBlendFactor(blendState.SrcFactorRGB);
    rpDesc.depthAttachmentPixelFormat = mtlTypes::asRenderTargetDepthFormat(blendState.DepthFormat);
    rpDesc.stencilAttachmentPixelFormat = mtlTypes::asRenderTargetStencilFormat(blendState.DepthFormat);
    rpDesc.fragmentFunction = pip.shd->mtlFragmentShader;
    rpDesc.vertexFunction = pip.shd->mtlVertexShader;
    rpDesc.vertexDescriptor = vtxDesc;
    rpDesc.rasterizationEnabled = YES;
    rpDesc.alphaToCoverageEnabled = pip.Setup.RasterizerState.AlphaToCoverageEnabled;
    rpDesc.alphaToOneEnabled = NO;
    rpDesc.sampleCount = pip.Setup.RasterizerState.SampleCount;
    NSError* err = NULL;
    pip.mtlRenderPipelineState = [this->pointers.renderer->mtlDevice newRenderPipelineStateWithDescriptor:rpDesc error:&err];
    if (!pip.mtlRenderPipelineState) {
        o_error("mtlDrawStateFactory: failed to create MTLRenderPipelineState with:\n  %s\n", err.localizedDescription.UTF8String);
    }
}

//------------------------------------------------------------------------------
void
mtlPipelineFactory::createDSS(pipeline& pip) {
    o_assert_dbg(nil == pip.mtlDepthStencilState);

    // create depth-stencil-state
    const DepthStencilState& dss = pip.Setup.DepthStencilState;
    MTLDepthStencilDescriptor* dsDesc = [[MTLDepthStencilDescriptor alloc] init];
    dsDesc.depthCompareFunction = mtlTypes::asCompareFunc(dss.DepthCmpFunc);
    dsDesc.depthWriteEnabled = dss.DepthWriteEnabled;
    if (dss.StencilEnabled) {
        dsDesc.backFaceStencil = [[MTLStencilDescriptor alloc] init];
        dsDesc.backFaceStencil.stencilFailureOperation = mtlTypes::asStencilOp(dss.StencilBack.FailOp);
        dsDesc.backFaceStencil.depthFailureOperation = mtlTypes::asStencilOp(dss.StencilBack.DepthFailOp);
        dsDesc.backFaceStencil.depthStencilPassOperation = mtlTypes::asStencilOp(dss.StencilBack.PassOp);
        dsDesc.backFaceStencil.stencilCompareFunction = mtlTypes::asCompareFunc(dss.StencilBack.CmpFunc);
        dsDesc.backFaceStencil.readMask = dss.StencilReadMask;
        dsDesc.backFaceStencil.writeMask = dss.StencilWriteMask;

        dsDesc.frontFaceStencil = [[MTLStencilDescriptor alloc] init];
        dsDesc.frontFaceStencil.stencilFailureOperation = mtlTypes::asStencilOp(dss.StencilFront.FailOp);
        dsDesc.frontFaceStencil.depthFailureOperation = mtlTypes::asStencilOp(dss.StencilFront.DepthFailOp);
        dsDesc.frontFaceStencil.depthStencilPassOperation = mtlTypes::asStencilOp(dss.StencilFront.PassOp);
        dsDesc.frontFaceStencil.stencilCompareFunction = mtlTypes::asCompareFunc(dss.StencilFront.CmpFunc);
        dsDesc.frontFaceStencil.readMask = dss.StencilReadMask;
        dsDesc.frontFaceStencil.writeMask = dss.StencilWriteMask;
    }
    pip.mtlDepthStencilState = [this->pointers.renderer->mtlDevice newDepthStencilStateWithDescriptor:dsDesc];
    o_assert(nil != pip.mtlDepthStencilState);
}

} // namespace _priv
} // namespace Oryol
