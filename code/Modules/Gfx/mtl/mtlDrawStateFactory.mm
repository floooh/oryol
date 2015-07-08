//------------------------------------------------------------------------------
//  mtlDrawStateFactory.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlDrawStateFactory.h"
#include "mtl_impl.h"
#include "mtlTypes.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
ResourceState::Code
mtlDrawStateFactory::SetupResource(drawState& ds) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nil == ds.mtlRenderPipelineState);
    o_assert_dbg(nil == ds.mtlDepthStencilState);
    o_assert_dbg(this->renderer && this->renderer->mtlDevice);

    drawStateFactoryBase::SetupResource(ds);
    o_assert_dbg(ds.prog);

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
    ds.mtlDepthStencilState = [this->renderer->mtlDevice newDepthStencilStateWithDescriptor:dsDesc];
    o_assert(nil != ds.mtlDepthStencilState);

    // create renderpipeline-state

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlDrawStateFactory::DestroyResource(drawState& ds) {
    o_assert_dbg(this->isValid);

    // ARC should take care about releasing the object when nil-ed
    this->renderer->invalidateDrawState();

    drawStateFactoryBase::DestroyResource(ds);
}

} // namespace _priv
} // namespace Oryol