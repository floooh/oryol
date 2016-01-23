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

    drawStateFactoryBase::SetupResource(ds);
    o_assert_dbg(ds.shd);

    // create MTLRenderPipelineState and MTLDepthStencilState
    createRPS(ds);
    createDSS(ds);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlDrawStateFactory::DestroyResource(drawState& ds) {
    o_assert_dbg(this->isValid);
    if (nil != ds.mtlRenderPipelineState) {
        releaseRPS(ds);
    }
    if (nil != ds.mtlDepthStencilState) {
        releaseDSS(ds);
    }
    drawStateFactoryBase::DestroyResource(ds);
}

//------------------------------------------------------------------------------
mtlDrawStateFactory::RPSKey
mtlDrawStateFactory::initRPSKey(const drawState& ds) {
    VertexLayout mergedLayout;
    for (const mesh* msh : ds.meshes) {
        if (msh) {
            mergedLayout.Append(msh->vertexBufferAttrs.Layout);
        }
    }
    RPSKey key;
    key.blendStateHash = ds.Setup.BlendState.Hash;
    key.vertexLayoutHash = mergedLayout.Hash();
    key.shader = ds.Setup.Shader;
    key.shaderSelectionMask = ds.Setup.ShaderSelectionMask;
    key.rasterizerStateHash = ds.Setup.RasterizerState.Hash;
    key.blendColor = ds.Setup.BlendColor;
    return key;
}

//------------------------------------------------------------------------------
void
mtlDrawStateFactory::createRPS(drawState& ds) {
    o_assert_dbg(nil == ds.mtlRenderPipelineState);

    // first check if identical state already exists
    const RPSKey key = this->initRPSKey(ds);
    const int32 cacheIndex = this->rpsCache.FindIndex(key);
    if (InvalidIndex != cacheIndex) {
        // re-use existing render-pipeline-state
        RPSValue& item = this->rpsCache.ValueAtIndex(cacheIndex);
        o_assert_dbg(this->rpsCache.KeyAtIndex(cacheIndex) == key);
        item.useCount++;
        ds.mtlRenderPipelineState = item.rps;
        o_dbg("mtlDrawStateFactory: re-use RenderPipelineState at %p\n", ds.mtlRenderPipelineState);
    }
    else {
        // create vertex-descriptor object
        MTLVertexDescriptor* vtxDesc = [MTLVertexDescriptor vertexDescriptor];
        for (int mshIndex = 0; mshIndex < GfxConfig::MaxNumInputMeshes; mshIndex++) {
            // NOTE: vertex buffers are located after constant buffers
            const int vbSlotIndex = mshIndex + GfxConfig::MaxNumUniformBlocksPerStage;
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
        const int32 progIndex = ds.shd->getProgIndexByMask(ds.Setup.ShaderSelectionMask);
        o_assert_dbg(InvalidIndex != progIndex);
        rpDesc.fragmentFunction = ds.shd->getFragmentShader(progIndex);
        rpDesc.vertexFunction = ds.shd->getVertexShader(progIndex);
        rpDesc.vertexDescriptor = vtxDesc;
        rpDesc.rasterizationEnabled = YES;
        rpDesc.alphaToCoverageEnabled = ds.Setup.RasterizerState.AlphaToCoverageEnabled;
        rpDesc.alphaToOneEnabled = NO;
        rpDesc.sampleCount = ds.Setup.RasterizerState.SampleCount;
        NSError* err = NULL;
        ds.mtlRenderPipelineState = [this->pointers.renderer->mtlDevice newRenderPipelineStateWithDescriptor:rpDesc error:&err];
        if (!ds.mtlRenderPipelineState) {
            o_error("mtlDrawStateFactory: failed to create MTLRenderPipelineState with:\n  %s\n", err.localizedDescription.UTF8String);
        }
        o_dbg("mtlDrawStateFactory: create new mtlRenderPipelineState at %p\n", ds.mtlRenderPipelineState);

        // add new entry to state-cache
        RPSValue item;
        item.rps = ds.mtlRenderPipelineState;
        item.useCount = 1;
        this->rpsCache.Add(key, item);
    }
}

//------------------------------------------------------------------------------
void
mtlDrawStateFactory::createDSS(drawState& ds) {
    o_assert_dbg(nil == ds.mtlDepthStencilState);

    // first check if identical state already exists
    const int32 cacheIndex = this->dssCache.FindIndex(ds.Setup.DepthStencilState.Hash);
    if (InvalidIndex != cacheIndex) {
        // re-use existing depth-stencil-state
        DSSValue& item = this->dssCache.ValueAtIndex(cacheIndex);
        o_assert_dbg(this->dssCache.KeyAtIndex(cacheIndex) == ds.Setup.DepthStencilState.Hash);
        item.useCount++;
        ds.mtlDepthStencilState = item.dss;
        o_dbg("mtlDrawStateFactory: re-use DepthStencilState at %p\n", ds.mtlDepthStencilState);
    }
    else {
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
        o_dbg("mtlDrawStateFactory: create new mtlDepthStencilState at %p\n", ds.mtlDepthStencilState);

        // add new entry to depth-stencil-state-cache
        DSSValue item;
        item.dss = ds.mtlDepthStencilState;
        item.useCount = 1;
        this->dssCache.Add(ds.Setup.DepthStencilState.Hash, item);
    }
}

//------------------------------------------------------------------------------
void
mtlDrawStateFactory::releaseRPS(drawState& ds) {
    o_assert_dbg(nil != ds.mtlRenderPipelineState);

    // find cache entry (FIXME: remove linear search?)
    if (this->rpsCache.Size() > 64) {
        o_warn("mtlDrawStateFactory::releaseRPS() FIXME: many cache items, remove linear search!\n");
    }
    for (int32 index = this->rpsCache.Size()-1; index >= 0; index--) {
        RPSValue* item = &this->rpsCache.ValueAtIndex(index);
        if (item->rps == ds.mtlRenderPipelineState) {
            o_assert_dbg(item->useCount > 0);
            if (--item->useCount == 0) {
                o_dbg("mtlDrawStateFactory: destroy MTLRenderPipelineState at %p\n", item->rps);
                this->pointers.renderer->releaseDeferred(item->rps);
                this->rpsCache.EraseIndex(index);
                item = nullptr;
            }
        }
    }
}

//------------------------------------------------------------------------------
void
mtlDrawStateFactory::releaseDSS(drawState& ds) {
    o_assert_dbg(nil != ds.mtlDepthStencilState);

    // find cache entry (FIXME: remove linear search?)
    if (this->dssCache.Size() > 64) {
        o_warn("mtlDrawStateFactory::releaseDSS() FIXME: many cache items, remove linear search!\n");
    }
    for (int32 index = this->dssCache.Size()-1; index >= 0; index--) {
        DSSValue* item = &this->dssCache.ValueAtIndex(index);
        if (item->dss == ds.mtlDepthStencilState) {
            o_assert_dbg(item->useCount > 0);
            if (--item->useCount == 0) {
                o_dbg("mtlDrawStateFactory: destroy MTLDepthStencilState at %p\n", item->dss);
                this->pointers.renderer->releaseDeferred(item->dss);
                this->dssCache.EraseIndex(index);
                item = nullptr;
            }
        }
    }
}

} // namespace _priv
} // namespace Oryol