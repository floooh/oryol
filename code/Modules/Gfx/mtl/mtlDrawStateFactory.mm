//------------------------------------------------------------------------------
//  mtlDrawStateFactory.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlDrawStateFactory.h"
#include "mtl_impl.h"
#include "mtlTypes.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
ResourceState::Code
mtlDrawStateFactory::SetupResource(drawState& ds) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nil == ds.mtlRenderPipelineState);
    o_assert_dbg(nil == ds.mtlDepthStencilState);

    drawStateFactoryBase::SetupResource(ds);
    o_assert_dbg(ds.shd);

    // create depth-stencil-state
    const DepthStencilState& dss = ds.Setup.DepthStencilState;
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
    ds.mtlDepthStencilState = [this->pointers.renderer->mtlDevice newDepthStencilStateWithDescriptor:dsDesc];
    o_assert(nil != ds.mtlDepthStencilState);

    // create vertex-descriptor object
    MTLVertexDescriptor* vtxDesc = [MTLVertexDescriptor vertexDescriptor];
    for (int mshIndex = 0; mshIndex < GfxConfig::MaxNumInputMeshes; mshIndex++) {
        // NOTE: vertex buffers are located after constant buffers
        const int vbSlotIndex = mshIndex + GfxConfig::MaxNumUniformBlocks;
        const mesh* msh = ds.meshes[mshIndex];
        if (msh) {
            const VertexLayout& layout = msh->vertexBufferAttrs.Layout;
            for (int compIndex = 0; compIndex < layout.NumComponents(); compIndex++) {
                const auto& comp = layout.ComponentAt(compIndex);
                vtxDesc.attributes[comp.Attr].format = mtlTypes::asVertexFormat(comp.Format);
                vtxDesc.attributes[comp.Attr].bufferIndex = vbSlotIndex;
                vtxDesc.attributes[comp.Attr].offset = layout.ComponentByteOffset(compIndex);
            }
            vtxDesc.layouts[vbSlotIndex].stride = layout.ByteSize();
            vtxDesc.layouts[vbSlotIndex].stepFunction = mtlTypes::asVertexStepFunc(msh->vertexBufferAttrs.StepFunction);
            vtxDesc.layouts[vbSlotIndex].stepRate = msh->vertexBufferAttrs.StepRate;
        }
    }

    // create renderpipeline-state
    const BlendState& blendState = ds.Setup.BlendState;
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
    o_warn("FIXME: move shader selection into DrawStateSetup!!!\n");
    rpDesc.fragmentFunction = ds.shd->getFragmentShaderAt(0);
    rpDesc.vertexFunction = ds.shd->getVertexShaderAt(0);
    rpDesc.vertexDescriptor = vtxDesc;
    rpDesc.rasterizationEnabled = YES;
    rpDesc.alphaToCoverageEnabled = ds.Setup.RasterizerState.AlphaToCoverageEnabled;
    rpDesc.alphaToOneEnabled = NO;
    rpDesc.sampleCount = ds.Setup.RasterizerState.SampleCount;
    NSError* err = NULL;
    ds.mtlRenderPipelineState = [this->pointers.renderer->mtlDevice newRenderPipelineStateWithDescriptor:rpDesc error:&err];
    if (!ds.mtlRenderPipelineState) {
        o_error("mtlDrawStateFactory: failed to create MTLRenderPipelineState with:\n  %s\n", err.localizedDescription.UTF8String);
        return ResourceState::InvalidState;
    }

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlDrawStateFactory::DestroyResource(drawState& ds) {
    o_assert_dbg(this->isValid);

    if (nil != ds.mtlRenderPipelineState) {
        ORYOL_OBJC_RELEASE(ds.mtlRenderPipelineState);
        ds.mtlRenderPipelineState = nil;
    }
    if (nil != ds.mtlDepthStencilState) {
        ORYOL_OBJC_RELEASE(ds.mtlDepthStencilState);
        ds.mtlDepthStencilState = nil;
    }

    drawStateFactoryBase::DestroyResource(ds);
}

} // namespace _priv
} // namespace Oryol