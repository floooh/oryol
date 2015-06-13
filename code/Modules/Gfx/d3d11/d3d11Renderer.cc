//------------------------------------------------------------------------------
//  d3d11Renderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11Renderer.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/Attrs/TextureAttrs.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11Renderer::d3d11Renderer() :
d3d11Device(nullptr),
d3d11DeviceContext(nullptr),
valid(false),
dispMgr(nullptr),
mshPool(nullptr),
texPool(nullptr),
defaultRenderTargetView(nullptr),
defaultDepthStencilView(nullptr),
rtValid(false),
curRenderTarget(nullptr),
curDrawState(nullptr),
curRenderTargetView(nullptr),
curDepthStencilView(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
d3d11Renderer::~d3d11Renderer() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::setup(displayMgr* dispMgr_, meshPool* mshPool_, texturePool* texPool_) {
    o_assert_dbg(!this->valid);
    o_assert_dbg(dispMgr_);
    o_assert_dbg(mshPool_);
    o_assert_dbg(texPool_);
    o_assert_dbg(dispMgr_->d3d11Device);
    o_assert_dbg(dispMgr_->d3d11DeviceContext);
    o_assert_dbg(dispMgr_->renderTargetView);
    o_assert_dbg(dispMgr_->depthStencilView);

    this->valid = true;
    this->dispMgr = dispMgr_;
    this->mshPool = mshPool_;
    this->texPool = texPool_;
    this->d3d11Device = this->dispMgr->d3d11Device;
    this->d3d11DeviceContext = this->dispMgr->d3d11DeviceContext;
    this->defaultRenderTargetView = this->dispMgr->renderTargetView;
    this->defaultDepthStencilView = this->dispMgr->depthStencilView;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::discard() {
    o_assert_dbg(this->valid);

    this->curRenderTargetView = nullptr;
    this->curDepthStencilView = nullptr;

    this->curRenderTarget = nullptr;
    this->curDrawState = nullptr;

    this->defaultDepthStencilView = nullptr;
    this->defaultRenderTargetView = nullptr;
    this->d3d11DeviceContext = nullptr;
    this->d3d11Device = nullptr;
    
    this->texPool = nullptr;
    this->mshPool = nullptr;
    this->dispMgr = nullptr;
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
d3d11Renderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::resetStateCache() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
bool
d3d11Renderer::supports(GfxFeature::Code feat) const {
    o_error("FIXME!\n");
    return false;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::commitFrame() {
    o_assert_dbg(this->valid);
    this->rtValid = false;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
d3d11Renderer::renderTargetAttrs() const {
    return this->rtAttrs;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyRenderTarget(texture* rt) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->dispMgr);
    o_assert_dbg(this->d3d11DeviceContext);

    if (nullptr == rt) {
        this->rtAttrs = this->dispMgr->GetDisplayAttrs();
        this->curRenderTargetView = this->defaultRenderTargetView;
        this->curDepthStencilView = this->defaultDepthStencilView;
    }
    else {
        // FIXME: hmm, have a 'AsDisplayAttrs' util function somewhere?
        const TextureAttrs& attrs = rt->textureAttrs;
        this->rtAttrs.WindowWidth = attrs.Width;
        this->rtAttrs.WindowHeight = attrs.Height;
        this->rtAttrs.WindowPosX = 0;
        this->rtAttrs.WindowPosY = 0;
        this->rtAttrs.FramebufferWidth = attrs.Width;
        this->rtAttrs.FramebufferHeight = attrs.Height;
        this->rtAttrs.ColorPixelFormat = attrs.ColorFormat;
        this->rtAttrs.DepthPixelFormat = attrs.DepthFormat;
        this->rtAttrs.Samples = 1;
        this->rtAttrs.Windowed = false;
        this->rtAttrs.SwapInterval = 1;

        o_error("FIXME: SET curRenderTargetView/curDepthStencilView!\n");
    }

    this->curRenderTarget = rt;
    this->rtValid = true;
    
    // actually set the render targets in the d3d11 device context
    this->d3d11DeviceContext->OMSetRenderTargets(1, &this->curRenderTargetView, this->curDepthStencilView);

    // set viewport to cover whole screen
    this->applyViewPort(0, 0, this->rtAttrs.FramebufferWidth, this->rtAttrs.FramebufferHeight);

    // FIXME: reset scissor test (or rather: apply a default rasterizer state)
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::applyViewPort(int32 x, int32 y, int32 width, int32 height) {
    o_assert_dbg(this->d3d11DeviceContext);

    D3D11_VIEWPORT vp;
    vp.TopLeftX = (FLOAT) x;
    vp.TopLeftY = (FLOAT) y;
    vp.Width    = (FLOAT) width;
    vp.Height   = (FLOAT) height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    this->d3d11DeviceContext->RSSetViewports(1, &vp);
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::applyScissorRect(int32 x, int32 y, int32 width, int32 height) {
    o_assert_dbg(this->d3d11DeviceContext);

    D3D11_RECT rect;
    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;
    this->d3d11DeviceContext->RSSetScissorRects(1, &rect);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyDrawState(drawState* ds) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(this->mshPool);
    o_assert_dbg(ds->d3d11DepthStencilState);
    o_assert_dbg(ds->d3d11RasterizerState);
    o_assert_dbg(ds->d3d11RasterizerState);

    if (nullptr == ds) {
        // the drawstate is still pending, invalidate rendering
        this->curDrawState = nullptr;
    }
    else {
        this->curDrawState = ds;
        o_assert_dbg(ds->prog);
        ds->prog->select(ds->Setup.ProgramSelectionMask);

        // apply state objects
        const UINT stencilRef = ds->Setup.DepthStencilState.StencilRef;
        FLOAT d3d11BlendFactor[4] = {
            ds->Setup.BlendColor.x,
            ds->Setup.BlendColor.y,
            ds->Setup.BlendColor.z,
            ds->Setup.BlendColor.w
        };
        this->d3d11DeviceContext->RSSetState(ds->d3d11RasterizerState);
        this->d3d11DeviceContext->OMSetDepthStencilState(ds->d3d11DepthStencilState, stencilRef);
        this->d3d11DeviceContext->OMSetBlendState(ds->d3d11BlendState, d3d11BlendFactor, 0xFFFFFFFF);

        // apply input assembly state (except primitive topology)
        // FIXME: these arrays should be stored in the d3d11DrawState!
        ID3D11Buffer* d3d11IABuffers[DrawStateSetup::MaxInputMeshes];
        UINT d3d11IAStrides[DrawStateSetup::MaxInputMeshes];
        UINT d3d11IAOffsets[DrawStateSetup::MaxInputMeshes];
        int d3d11NumBuffers = 0;
        for (int mshIndex = 0; mshIndex < DrawStateSetup::MaxInputMeshes; mshIndex++) {
            const mesh* msh = ds->meshes[mshIndex];
            if (msh) {
                d3d11IABuffers[d3d11NumBuffers] = msh->d3d11VertexBuffer;
                d3d11IAStrides[d3d11NumBuffers] = msh->vertexBufferAttrs.Layout.ByteSize();
                d3d11IAOffsets[d3d11NumBuffers] = 0;
                d3d11NumBuffers++;
            }
        }
        this->d3d11DeviceContext->IASetVertexBuffers(0, d3d11NumBuffers, d3d11IABuffers, d3d11IAStrides, d3d11IAOffsets);

        if (ds->meshes[0]->d3d11IndexBuffer) {
            DXGI_FORMAT d3d11IndexFormat = (DXGI_FORMAT) ds->meshes[0]->indexBufferAttrs.Type;
            this->d3d11DeviceContext->IASetIndexBuffer(ds->meshes[0]->d3d11IndexBuffer, d3d11IndexFormat, 0);
        }

        const uint32 selIndex = ds->prog->getSelectionIndex();
        o_assert_dbg(ds->d3d11InputLayouts[selIndex]);
        this->d3d11DeviceContext->IASetInputLayout(ds->d3d11InputLayouts[selIndex]);

        // apply vertex stage state
        this->d3d11DeviceContext->VSSetShader(ds->prog->getSelectedVertexShader(), NULL, 0);

        // apply pixel stage state
        this->d3d11DeviceContext->PSSetShader(ds->prog->getSelectedPixelShader(), NULL, 0);
    }
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyUniformBlock(int32 blockIndex, int64 layoutHash, const uint8* ptr, int32 byteSize) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::clear(ClearTarget::Mask clearMask, const glm::vec4& color, float32 depth, uint8 stencil) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert2_dbg((clearMask & ClearTarget::All) != 0, "No clear flags set (note that this has changed from PixelChannel)\n");

    if (clearMask & ClearTarget::Color) {
        o_assert_dbg(this->curRenderTargetView);
        this->d3d11DeviceContext->ClearRenderTargetView(this->curRenderTargetView, glm::value_ptr(color));
    }
    if (clearMask & (ClearTarget::Depth|ClearTarget::Stencil)) {
        o_assert_dbg(this->curDepthStencilView);
        UINT d3d11ClearFlags = 0;
        if (ClearTarget::Depth & clearMask) {
            d3d11ClearFlags |= D3D11_CLEAR_DEPTH;
        }
        if (ClearTarget::Stencil & clearMask) {
            d3d11ClearFlags |= D3D11_CLEAR_STENCIL;
        }
        this->d3d11DeviceContext->ClearDepthStencilView(this->curDepthStencilView, d3d11ClearFlags, depth, stencil);
    }
}

//------------------------------------------------------------------------------
void
d3d11Renderer::draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert2_dbg(this->rtValid, "No render target set!\n");

    // only render if a valid draw state had been set
    if (this->curDrawState) {
        o_assert_dbg(this->curDrawState->meshes[0]);

        D3D11_PRIMITIVE_TOPOLOGY primTop = (D3D11_PRIMITIVE_TOPOLOGY)primGroup.PrimType;
        this->d3d11DeviceContext->IASetPrimitiveTopology(primTop);
        const IndexType::Code indexType = this->curDrawState->meshes[0]->indexBufferAttrs.Type;
        if (indexType != IndexType::None) {
            // indexed geometry
            this->d3d11DeviceContext->DrawIndexed(primGroup.NumElements, primGroup.BaseElement, 0);
        }
        else {
            this->d3d11DeviceContext->Draw(primGroup.NumElements, primGroup.BaseElement);        
        }
    }
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::draw(int32 primGroupIndex) {
    o_assert_dbg(this->valid);

    // only render if a valid draw state had been set
    if (this->curDrawState) {
        o_assert_dbg(this->curDrawState->meshes[0]);
        if (primGroupIndex >= this->curDrawState->meshes[0]->numPrimGroups) {
            // this may happen if trying to render a placeholder which doesn't
            // have as many materials as the original mesh, anyway, this isn't
            // a serious error
            return;
        }
        const PrimitiveGroup& primGroup = this->curDrawState->meshes[0]->primGroups[primGroupIndex];
        this->draw(primGroup);
    }
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::drawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::updateVertices(mesh* msh, const void* data, int32 numBytes) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::readPixels(displayMgr* displayManager, void* buf, int32 bufNumBytes) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11Renderer::invalidateMeshState() {
    Log::Info("d3d11Renderer::invalidateMeshState()\n");
}

//------------------------------------------------------------------------------
void
d3d11Renderer::invalidateProgramState() {
    Log::Info("d3d11Renderer::invalidateProgramState()\n");
}

//------------------------------------------------------------------------------
void
d3d11Renderer::invalidateDrawState() {
    Log::Info("d3d11Renderer::invalidateDrawState()\n");
}

} // namespace _priv
} // namespace Oryol
