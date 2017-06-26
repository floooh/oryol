//------------------------------------------------------------------------------
//  d3d11Renderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11Renderer.h"
#include "Gfx/private/displayMgr.h"
#include "Gfx/private/resourcePools.h"
#include "Gfx/GfxTypes.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "d3d11_impl.h"
#include "d3d11Types.h"
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <algorithm>    // std::max()

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11Renderer::d3d11Renderer() :
d3d11Device(nullptr),
d3d11DeviceContext(nullptr),
valid(false),
rpValid(false),
frameIndex(0),
curRenderPass(nullptr),
curPipeline(nullptr),
curPrimaryMesh(nullptr),
numRTVs(0),
d3d11CurDSV(nullptr),
d3d11CurRasterizerState(nullptr),
d3d11CurDepthStencilState(nullptr),
d3d11CurBlendState(nullptr),
d3d11CurIndexBuffer(nullptr),
d3d11CurInputLayout(nullptr),
d3d11CurVertexShader(nullptr),
d3d11CurPixelShader(nullptr),
d3d11CurPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED),
curStencilRef(0xFFFF) {
    this->d3d11CurRTVs.Fill(nullptr);
    this->d3d11CurVSCBs.Fill(nullptr);
    this->d3d11CurPSCBs.Fill(nullptr);
    this->d3d11CurVBs.Fill(nullptr);
    this->d3d11CurVSSRVs.Fill(nullptr);
    this->d3d11CurPSSRVs.Fill(nullptr);
    this->d3d11CurVSSamplers.Fill(nullptr);
    this->d3d11CurPSSamplers.Fill(nullptr);
    this->curVertexStrides.Fill(0);
    this->curVertexOffsets.Fill(0);
}

//------------------------------------------------------------------------------
d3d11Renderer::~d3d11Renderer() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert_dbg(!this->valid);

    this->valid = true;
    this->gfxSetup = setup;
    this->pointers = ptrs;
    this->d3d11Device = this->pointers.displayMgr->d3d11Device;
    this->d3d11DeviceContext = this->pointers.displayMgr->d3d11DeviceContext;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::discard() {
    o_assert_dbg(this->valid);

    this->numRTVs = 0;
    this->d3d11CurRTVs.Fill(nullptr);
    this->d3d11CurDSV = nullptr;
    this->d3d11CurRasterizerState = nullptr;
    this->d3d11CurDepthStencilState = nullptr;
    this->d3d11CurBlendState = nullptr;
    this->d3d11CurIndexBuffer = nullptr;
    this->d3d11CurInputLayout = nullptr;
    this->d3d11CurVertexShader = nullptr;
    this->d3d11CurPixelShader = nullptr;
    this->d3d11CurPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    this->d3d11CurVSCBs.Fill(nullptr);
    this->d3d11CurPSCBs.Fill(nullptr);
    this->d3d11CurVBs.Fill(nullptr);
    this->d3d11CurVSSRVs.Fill(nullptr);
    this->d3d11CurPSSRVs.Fill(nullptr);
    this->d3d11CurVSSamplers.Fill(nullptr);
    this->d3d11CurPSSamplers.Fill(nullptr);
    this->curVertexStrides.Fill(0);
    this->curVertexOffsets.Fill(0);

    this->curRenderPass = nullptr;
    this->curPipeline = nullptr;
    this->curPrimaryMesh = nullptr;

    this->d3d11DeviceContext = nullptr;
    this->d3d11Device = nullptr;
    
    this->pointers = gfxPointers();
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
    o_assert_dbg(this->d3d11DeviceContext);

    this->d3d11DeviceContext->ClearState();
    this->curRenderPass = nullptr;
    this->curPipeline = nullptr;
    this->numRTVs = 0;
    this->d3d11CurRTVs.Fill(nullptr);
    this->d3d11CurDSV = nullptr;
    this->d3d11CurDepthStencilState = nullptr;
    this->d3d11CurRasterizerState = nullptr;
    this->d3d11CurBlendState = nullptr;
    this->d3d11CurIndexBuffer = nullptr;
    this->d3d11CurInputLayout = nullptr;
    this->d3d11CurVertexShader = nullptr;
    this->d3d11CurPixelShader = nullptr;
    this->d3d11CurPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    this->d3d11CurVSCBs.Fill(nullptr);
    this->d3d11CurPSCBs.Fill(nullptr);
    this->d3d11CurVBs.Fill(nullptr);
    this->d3d11CurVSSRVs.Fill(nullptr);
    this->d3d11CurPSSRVs.Fill(nullptr);
    this->d3d11CurVSSamplers.Fill(nullptr);
    this->d3d11CurPSSamplers.Fill(nullptr);
    this->curVertexStrides.Fill(0);
    this->curVertexOffsets.Fill(0);
    this->curStencilRef = 0xFFFF;
    this->curBlendColor = glm::vec4(0.0f);
}

//------------------------------------------------------------------------------
bool
d3d11Renderer::queryFeature(GfxFeature::Code feat) const {
    switch (feat) {
        case GfxFeature::TextureCompressionDXT:
        case GfxFeature::TextureFloat:
        case GfxFeature::Instancing:
        case GfxFeature::OriginTopLeft:
        case GfxFeature::MSAARenderTargets:
        case GfxFeature::MultipleRenderTarget:
        case GfxFeature::Texture3D:
        case GfxFeature::TextureArray:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
void
d3d11Renderer::commitFrame() {
    o_assert_dbg(this->valid);
    this->rpValid = false;
    this->curRenderPass = nullptr;
    this->curPipeline = nullptr;
    this->curPrimaryMesh = nullptr;
    this->frameIndex++;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
d3d11Renderer::renderPassAttrs() const {
    return this->rpAttrs;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::beginPass(renderPass* pass, const PassAction* action) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(action);

    // don't keep texture binding across passes, bound texture might be render targets!
    this->invalidateTextureState();
    this->d3d11CurRTVs.Fill(nullptr);
    if (nullptr == pass) {
        this->rpAttrs = this->pointers.displayMgr->GetDisplayAttrs();
        this->numRTVs = 1;
        this->d3d11CurRTVs[0] = this->pointers.displayMgr->d3d11RenderTargetView;
        this->d3d11CurDSV = this->pointers.displayMgr->d3d11DepthStencilView;
    }
    else {
        o_assert_dbg(pass->colorTextures[0]);
        this->rpAttrs = DisplayAttrs::FromTextureAttrs(pass->colorTextures[0]->textureAttrs);
        int i;
        for (i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
            if (pass->d3d11RenderTargetViews[i]) {
                this->d3d11CurRTVs[i] = pass->d3d11RenderTargetViews[i];
            }
            else {
                break;
            }
        }
        this->numRTVs = i;
        this->d3d11CurDSV = pass->d3d11DepthStencilView; 
    }
    o_assert_dbg(nullptr == this->curRenderPass);
    this->curRenderPass = pass;
    this->rpValid = true;

    // actually set the render targets in the d3d11 device context
    this->d3d11DeviceContext->OMSetRenderTargets(this->numRTVs, &(this->d3d11CurRTVs[0]), this->d3d11CurDSV);

    // set viewport to cover whole screen
    this->applyViewPort(0, 0, this->rpAttrs.FramebufferWidth, this->rpAttrs.FramebufferHeight, true);

    // perform clear action
    if (nullptr == this->curRenderPass) {
        if (action->Flags & PassAction::ClearC0) {
            if (this->d3d11CurRTVs[0]) {
                this->d3d11DeviceContext->ClearRenderTargetView(this->d3d11CurRTVs[0], glm::value_ptr(action->Color[0]));
            }
        }
        if (action->Flags & PassAction::ClearDS) {
            if (this->d3d11CurDSV) {
                const UINT f = D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL;
                this->d3d11DeviceContext->ClearDepthStencilView(this->d3d11CurDSV, f, action->Depth, action->Stencil);
            }
        }
    }
    else {
        for (int i = 0; i < this->numRTVs; i++) {
            if (this->d3d11CurRTVs[i]) {
                if (action->Flags & (PassAction::ClearC0<<i)) {
                    this->d3d11DeviceContext->ClearRenderTargetView(this->d3d11CurRTVs[i], glm::value_ptr(action->Color[i]));
                }
            }
            else {
                break;
            }
        }
        if (this->d3d11CurDSV && (action->Flags & PassAction::ClearDS)) { 
            const UINT f = D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL;
            this->d3d11DeviceContext->ClearDepthStencilView(this->d3d11CurDSV, f, action->Depth, action->Stencil);
        }
    }
}

//------------------------------------------------------------------------------
void
d3d11Renderer::endPass() {
    o_assert_dbg(this->valid);

    const renderPass* rp = this->curRenderPass;
    if (rp) {
        const bool isMSAA = nullptr != rp->colorTextures[0]->d3d11MSAATexture2D;
        if (isMSAA) {
            // perform MSAA resolve on offscreen render targets
            for (int i = 0; i < this->numRTVs; i++) {
                texture* colorTex = rp->colorTextures[i];
                const auto& att = rp->Setup.ColorAttachments[i];
                o_assert_dbg(colorTex->d3d11MSAATexture2D && colorTex->d3d11Texture2D);
                UINT subres = D3D11CalcSubresource(att.MipLevel, att.Slice, colorTex->textureAttrs.NumMipMaps);
                this->d3d11DeviceContext->ResolveSubresource(
                    colorTex->d3d11Texture2D,       // pDstResource
                    subres,                         // DstSubresource
                    colorTex->d3d11MSAATexture2D,   // pSrcResource
                    subres,                         // SrcSubresource
                    colorTex->d3d11ColorFormat);
            }
        }
    }
    this->curRenderPass = nullptr;
    this->rpValid = false;
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::applyViewPort(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(this->d3d11DeviceContext);

    D3D11_VIEWPORT vp;
    vp.TopLeftX = (FLOAT) x;
    vp.TopLeftY = (FLOAT) (originTopLeft ? y : (this->rpAttrs.FramebufferHeight - (y + height)));
    vp.Width    = (FLOAT) width;
    vp.Height   = (FLOAT) height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    this->d3d11DeviceContext->RSSetViewports(1, &vp);
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::applyScissorRect(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(this->d3d11DeviceContext);

    D3D11_RECT rect;
    rect.left = x;
    rect.top = originTopLeft ? y : this->rpAttrs.FramebufferHeight - (y + height);
    rect.right = x + width;
    rect.bottom = originTopLeft ? (y + height) : (this->rpAttrs.FramebufferHeight - y);
    this->d3d11DeviceContext->RSSetScissorRects(1, &rect);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyDrawState(pipeline* pip, mesh** meshes, int numMeshes) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(pip);
    o_assert_dbg(pip->shd);
    o_assert_dbg(meshes && numMeshes > 0);

    // if any of the meshes are still loading, cancel the next draw state
    for (int i = 0; i < numMeshes; i++) {
        if (nullptr == meshes[i]) {
            this->curPipeline = nullptr;
            return;
        }
    }
    o_assert_dbg(pip->d3d11DepthStencilState);
    o_assert_dbg(pip->d3d11RasterizerState);
    o_assert_dbg(pip->d3d11BlendState);
    #if ORYOL_DEBUG
    o_assert2(pip->Setup.BlendState.ColorFormat == this->rpAttrs.ColorPixelFormat, "ColorFormat in BlendState must match current render target!\n");
    o_assert2(pip->Setup.BlendState.DepthFormat == this->rpAttrs.DepthPixelFormat, "DepthFormat in BlendState must match current render target!\n");
    o_assert2(pip->Setup.RasterizerState.SampleCount == this->rpAttrs.SampleCount, "SampleCount in RasterizerState must match current render target!\n");
    if (this->curRenderPass) {
        for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
            const texture* tex = this->curRenderPass->colorTextures[i];
            if (tex) {
                o_assert2(pip->Setup.BlendState.ColorFormat == tex->textureAttrs.ColorFormat, "ColorFormat in BlendState must match MRT color attachments!\n");
                o_assert2(pip->Setup.RasterizerState.SampleCount == tex->textureAttrs.SampleCount, "SampleCount in RasterizerState must match MRT color attachments!\n");
            }
        }
        const texture* dsTex = this->curRenderPass->depthStencilTexture;
        if (dsTex) {
            o_assert2(pip->Setup.BlendState.DepthFormat == dsTex->textureAttrs.DepthFormat, "DepthFormat in BlendState must match depth/stencil attachment!\n");
        }
    }
    #endif
    this->curPipeline = pip;
    this->curPrimaryMesh = meshes[0];
    o_assert_dbg(this->curPrimaryMesh);

    // apply state objects (if state has changed)
    if (pip->d3d11RasterizerState != this->d3d11CurRasterizerState) {
        this->d3d11CurRasterizerState = pip->d3d11RasterizerState;
        this->d3d11DeviceContext->RSSetState(pip->d3d11RasterizerState);
    }
    if ((pip->d3d11DepthStencilState != this->d3d11CurDepthStencilState) ||
        (pip->Setup.DepthStencilState.StencilRef != this->curStencilRef)) {

        this->d3d11CurDepthStencilState = pip->d3d11DepthStencilState;
        this->curStencilRef = pip->Setup.DepthStencilState.StencilRef;
        this->d3d11DeviceContext->OMSetDepthStencilState(pip->d3d11DepthStencilState, pip->Setup.DepthStencilState.StencilRef);
    }
    if ((pip->d3d11BlendState != this->d3d11CurBlendState) ||
        glm::any(glm::notEqual(pip->Setup.BlendColor, this->curBlendColor))) {
        
        this->d3d11CurBlendState = pip->d3d11BlendState;
        this->curBlendColor = pip->Setup.BlendColor;
        this->d3d11DeviceContext->OMSetBlendState(pip->d3d11BlendState, glm::value_ptr(pip->Setup.BlendColor), 0xFFFFFFFF);
    }

    // apply vertex buffers
    bool vbDirty = false;
    for (int slotIndex = 0; slotIndex < GfxConfig::MaxNumInputMeshes; slotIndex++) {
        const mesh* msh = slotIndex < numMeshes ? meshes[slotIndex] : nullptr;
        if (msh) {
            if (this->d3d11CurVBs[slotIndex] != msh->d3d11VertexBuffer) {
                this->d3d11CurVBs[slotIndex] = msh->d3d11VertexBuffer;
                vbDirty = true;
            }
            if (this->curVertexStrides[slotIndex] != msh->vertexBufferAttrs.Layout.ByteSize()) {
                this->curVertexStrides[slotIndex] = msh->vertexBufferAttrs.Layout.ByteSize();
                vbDirty = true;
            }
        }
        else if (this->d3d11CurVBs[slotIndex] != nullptr) {
            vbDirty = true;
            this->d3d11CurVBs[slotIndex] = nullptr;
            this->curVertexStrides[slotIndex] = 0;
        }
    }
    if (vbDirty) {
        this->d3d11DeviceContext->IASetVertexBuffers(
            0,                                      // StartSlot 
            GfxConfig::MaxNumInputMeshes,           // NumBuffers
            &(this->d3d11CurVBs[0]),                // ppVertexBuffers
            &(this->curVertexStrides[0]),           // pStrides
            &(this->curVertexOffsets[0]));          // pOffsets
    }
    if (this->d3d11CurPrimitiveTopology != pip->d3d11PrimTopology) {
        this->d3d11CurPrimitiveTopology = pip->d3d11PrimTopology;
        this->d3d11DeviceContext->IASetPrimitiveTopology(pip->d3d11PrimTopology);
    }

    // apply optional index buffer (can be nullptr!)
    if (this->d3d11CurIndexBuffer != this->curPrimaryMesh->d3d11IndexBuffer) {
        this->d3d11CurIndexBuffer = this->curPrimaryMesh->d3d11IndexBuffer;
        DXGI_FORMAT d3d11IndexFormat = d3d11Types::asIndexType(this->curPrimaryMesh->indexBufferAttrs.Type);
        this->d3d11DeviceContext->IASetIndexBuffer(this->curPrimaryMesh->d3d11IndexBuffer, d3d11IndexFormat, 0);
    }

    // apply input layout and shaders
    if (this->d3d11CurInputLayout != pip->d3d11InputLayout) {
        this->d3d11CurInputLayout = pip->d3d11InputLayout;
        this->d3d11DeviceContext->IASetInputLayout(pip->d3d11InputLayout);
    }

    // apply shaders
    if (this->d3d11CurVertexShader != pip->shd->d3d11VertexShader) {
        this->d3d11CurVertexShader = pip->shd->d3d11VertexShader;
        this->d3d11DeviceContext->VSSetShader(pip->shd->d3d11VertexShader, NULL, 0);
    }
    if (this->d3d11CurPixelShader != pip->shd->d3d11PixelShader) {
        this->d3d11CurPixelShader = pip->shd->d3d11PixelShader;
        this->d3d11DeviceContext->PSSetShader(pip->shd->d3d11PixelShader, NULL, 0);
    }
        
    // apply vertex-shader-stage constant buffers
    for (int bindSlot = 0; bindSlot < GfxConfig::MaxNumUniformBlocksPerStage; bindSlot++) {
        // NOTE: cb can be nullptr!
        ID3D11Buffer* cb = pip->shd->getConstantBuffer(ShaderStage::VS, bindSlot);
        if (this->d3d11CurVSCBs[bindSlot] != cb) {
            this->d3d11CurVSCBs[bindSlot] = cb;
            this->d3d11DeviceContext->VSSetConstantBuffers(bindSlot, 1, &cb);
        }
    }

    // apply fragment-shader-stage constant buffers
    for (int bindSlot = 0; bindSlot < GfxConfig::MaxNumUniformBlocksPerStage; bindSlot++) {
        // NOTE: cb can be nullptr!
        ID3D11Buffer* cb = pip->shd->getConstantBuffer(ShaderStage::FS, bindSlot);
        if (this->d3d11CurPSCBs[bindSlot] != cb) {
            this->d3d11CurPSCBs[bindSlot] = cb;
            this->d3d11DeviceContext->PSSetConstantBuffers(bindSlot, 1, &cb);
        }
    }
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, uint32_t typeHash, const uint8_t* ptr, int byteSize) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(0 != typeHash);
    if (nullptr == this->curPipeline) {
        // currently no valid draw state set
        return;
    }

    const shader* shd = this->curPipeline->shd;
    o_assert_dbg(shd);
    
    #if ORYOL_DEBUG
    // check whether the provided struct is type-compatible with the
    // expected uniform-block-layout, the size-check shouldn't be necessary
    // since the hash should already bail out, but it doesn't hurt either
    int ubIndex = shd->Setup.UniformBlockIndexByStageAndSlot(bindStage, bindSlot);
    o_assert(InvalidIndex != ubIndex);
    const uint32_t ubTypeHash = shd->Setup.UniformBlockTypeHash(ubIndex);
    const int ubByteSize = shd->Setup.UniformBlockByteSize(ubIndex);
    o_assert(ubTypeHash == typeHash);
    o_assert(ubByteSize >= byteSize);
    #endif

    // NOTE: UpdateSubresource() and map-discard are equivalent (at least on nvidia)
    ID3D11Buffer* cb = shd->getConstantBuffer(bindStage, bindSlot);
    o_assert_dbg(cb);
    this->d3d11DeviceContext->UpdateSubresource(cb, 0, nullptr, ptr, 0, 0);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyTextures(ShaderStage::Code bindStage, texture** textures, int numTextures) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(this->valid);
    if (nullptr == this->curPipeline) {
        return;
    }

    // if any of the provided texture pointers are not valid, this means one of the
    // textures isn't valid yet, in this case, disable rendering for the next draw call
    for (int i = 0; i < numTextures; i++) {
        if (nullptr == textures[i]) {
            this->curPipeline = nullptr;
            return;
        }
    }

    // apply textures and samplers
    if (ShaderStage::VS == bindStage) {
        o_assert_dbg(numTextures <= GfxConfig::MaxNumVertexTextures);
        for (int i = 0; i < numTextures; i++) {
            if (textures[i]->d3d11ShaderResourceView != this->d3d11CurVSSRVs[i]) {
                this->d3d11CurVSSRVs[i] = textures[i]->d3d11ShaderResourceView;
                this->d3d11DeviceContext->VSSetShaderResources(i, 1, &(textures[i]->d3d11ShaderResourceView));
            }
            if (textures[i]->d3d11SamplerState != this->d3d11CurVSSamplers[i]) {
                this->d3d11CurVSSamplers[i] = textures[i]->d3d11SamplerState;
                this->d3d11DeviceContext->VSSetSamplers(i, 1, &(textures[i]->d3d11SamplerState));
            }
        }
    }
    else {
        o_assert_dbg(numTextures <= GfxConfig::MaxNumFragmentTextures);
        for (int i = 0; i < numTextures; i++) {
            if (textures[i]->d3d11ShaderResourceView != this->d3d11CurPSSRVs[i]) {
                this->d3d11CurPSSRVs[i] = textures[i]->d3d11ShaderResourceView;
                this->d3d11DeviceContext->PSSetShaderResources(i, 1, &(textures[i]->d3d11ShaderResourceView));
            }
            if (textures[i]->d3d11SamplerState != this->d3d11CurPSSamplers[i]) {
                this->d3d11CurPSSamplers[i] = textures[i]->d3d11SamplerState;
                this->d3d11DeviceContext->PSSetSamplers(i, 1, &(textures[i]->d3d11SamplerState));
            }
        }
    }
}

//------------------------------------------------------------------------------
void
d3d11Renderer::draw(int baseElementIndex, int numElements, int numInstances) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(numInstances >= 1);
    o_assert2_dbg(this->rpValid, "No render target set!\n");
    if (nullptr == this->curPipeline) {
        return;
    }
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    const IndexType::Code indexType = msh->indexBufferAttrs.Type;
    if (indexType != IndexType::None) {
        if (numInstances == 1) {
            this->d3d11DeviceContext->DrawIndexed(numElements, baseElementIndex, 0);
        }
        else {
            this->d3d11DeviceContext->DrawIndexedInstanced(numElements, numInstances, baseElementIndex, 0, 0);
        }
    }
    else {
        if (numInstances == 1) {
            this->d3d11DeviceContext->Draw(numElements, baseElementIndex);
        }
        else {
            this->d3d11DeviceContext->DrawInstanced(numElements, numInstances, baseElementIndex, 0);
        }
    }
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::draw(int primGroupIndex, int numInstances) {
    o_assert_dbg(this->valid);
    if (nullptr == this->curPipeline) {
        return;
    }
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    if (primGroupIndex >= msh->numPrimGroups) {
        // this may happen if trying to render a placeholder which doesn't
        // have as many materials as the original mesh, anyway, this isn't
        // a serious error
        return;
    }
    const PrimitiveGroup& primGroup = msh->primGroups[primGroupIndex];
    this->draw(primGroup.BaseElement, primGroup.NumElements, numInstances);
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::updateVertices(mesh* msh, const void* data, int numBytes) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(nullptr != msh);
    o_assert_dbg(msh->d3d11VertexBuffer);
    o_assert_dbg(numBytes > 0);

    o_assert_dbg((numBytes > 0) && (numBytes <= msh->vertexBufferAttrs.ByteSize()));
    o_assert_dbg(Usage::Immutable != msh->vertexBufferAttrs.BufferUsage);

    o_assert2(msh->vbUpdateFrameIndex != this->frameIndex, "Only one data update allowed per buffer and frame!\n");
    msh->vbUpdateFrameIndex = this->frameIndex;

    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = this->d3d11DeviceContext->Map(msh->d3d11VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    o_assert_dbg(SUCCEEDED(hr));
    std::memcpy(mapped.pData, data, numBytes);
    this->d3d11DeviceContext->Unmap(msh->d3d11VertexBuffer, 0);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::updateIndices(mesh* msh, const void* data, int numBytes) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(nullptr != msh);
    o_assert_dbg(msh->d3d11IndexBuffer);
    o_assert_dbg(numBytes > 0);

    o_assert_dbg((numBytes > 0) && (numBytes <= msh->indexBufferAttrs.ByteSize()));
    o_assert_dbg(Usage::Immutable != msh->indexBufferAttrs.BufferUsage);

    o_assert2(msh->ibUpdateFrameIndex != this->frameIndex, "Only one data update allowed per buffer and frame!\n");
    msh->ibUpdateFrameIndex = this->frameIndex;

    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = this->d3d11DeviceContext->Map(msh->d3d11IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    o_assert_dbg(SUCCEEDED(hr));
    std::memcpy(mapped.pData, data, numBytes);
    this->d3d11DeviceContext->Unmap(msh->d3d11IndexBuffer, 0);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSizes) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(tex);
    o_assert_dbg(data);
    o_assert_dbg(tex->d3d11Texture2D);

    // only accept 2D textures for now
    const TextureAttrs& attrs = tex->textureAttrs;
    o_assert_dbg(TextureType::Texture2D == attrs.Type);
    o_assert_dbg(Usage::Immutable != attrs.TextureUsage);
    o_assert_dbg(!PixelFormat::IsCompressedFormat(attrs.ColorFormat));
    o_assert_dbg(offsetsAndSizes.NumMipMaps == attrs.NumMipMaps);
    o_assert_dbg(offsetsAndSizes.NumFaces == 1);
    
    D3D11_MAPPED_SUBRESOURCE mapped;
    for (int mipIndex = 0; mipIndex < attrs.NumMipMaps; mipIndex++) {
        o_assert_dbg(offsetsAndSizes.Sizes[0][mipIndex] > 0);
        const int mipWidth = std::max(attrs.Width >> mipIndex, 1);
        const int mipHeight = std::max(attrs.Height >> mipIndex, 1);
        const int srcPitch = PixelFormat::RowPitch(attrs.ColorFormat, mipWidth);
        HRESULT hr = this->d3d11DeviceContext->Map(
            tex->d3d11Texture2D,        // pResource
            mipIndex,                   // Subresource
            D3D11_MAP_WRITE_DISCARD,    // MapType
            0,                          // MapFlags
            &mapped);                   // pMappedResource
        o_assert_dbg(SUCCEEDED(hr));
        o_assert_dbg(srcPitch <= (int)mapped.RowPitch);
        uint8_t* dstPtr = (uint8_t*)mapped.pData;
        const uint8_t* srcPtr = ((const uint8_t*)data) + offsetsAndSizes.Offsets[0][mipIndex];
        if (srcPitch == mapped.RowPitch) {
            const int mipSize = offsetsAndSizes.Sizes[0][mipIndex];
            o_assert_dbg(mipSize <= (srcPitch*mipHeight));
            std::memcpy(dstPtr, srcPtr, mipSize);
        }
        else {
            o_error("d3dRenderer::updateTexture(): srcPitch!=dstPitch, FIXME UNTESTED!\n");
            for (int rowIndex = 0; rowIndex < mipHeight; rowIndex++) {
                std::memcpy(dstPtr, srcPtr, srcPitch);
                dstPtr += mapped.RowPitch;
                srcPtr += srcPitch;
            }
        }
        this->d3d11DeviceContext->Unmap(tex->d3d11Texture2D, mipIndex);
    }
}

//------------------------------------------------------------------------------
void
d3d11Renderer::invalidateMeshState() {
    o_assert_dbg(this->d3d11DeviceContext);

    Log::Info("d3d11Renderer::invalidateMeshState()\n");

    this->d3d11CurIndexBuffer = nullptr;
    this->d3d11CurInputLayout = nullptr;
    this->d3d11CurVBs.Fill(nullptr);
    this->curVertexStrides.Fill(0);
    this->curVertexOffsets.Fill(0);
    this->d3d11CurPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    this->d3d11DeviceContext->IASetInputLayout(nullptr);
    this->d3d11DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    this->d3d11DeviceContext->IASetVertexBuffers(
        0, 
        this->d3d11CurVBs.Size(),
        &(this->d3d11CurVBs[0]),
        &(this->curVertexStrides[0]),
        &(this->curVertexOffsets[0]));
}

//------------------------------------------------------------------------------
void
d3d11Renderer::invalidateShaderState() {
    o_assert_dbg(this->d3d11DeviceContext);

    Log::Info("d3d11Renderer::invalidateProgramState()\n");

    this->d3d11CurVertexShader = nullptr;
    this->d3d11CurPixelShader = nullptr;
    this->d3d11CurVSCBs.Fill(nullptr);
    this->d3d11CurPSCBs.Fill(nullptr);
    this->d3d11DeviceContext->VSSetShader(nullptr, nullptr, 0);
    this->d3d11DeviceContext->PSSetShader(nullptr, nullptr, 0);
    this->d3d11DeviceContext->VSSetConstantBuffers(0, GfxConfig::MaxNumUniformBlocksPerStage, &this->d3d11CurVSCBs[0]);
    this->d3d11DeviceContext->PSSetConstantBuffers(0, GfxConfig::MaxNumUniformBlocksPerStage, &this->d3d11CurPSCBs[0]);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::invalidatePipeline() {
    o_assert_dbg(this->d3d11DeviceContext);

    Log::Info("d3d11Renderer::invalidatePipeline()\n");

    this->d3d11CurBlendState = nullptr;
    this->d3d11CurDepthStencilState = nullptr;
    this->d3d11CurRasterizerState = nullptr;
    this->d3d11DeviceContext->OMSetBlendState(nullptr, glm::value_ptr(this->curBlendColor), 0xFFFFFFFF);
    this->d3d11DeviceContext->OMSetDepthStencilState(nullptr, 0xFF);
    this->d3d11DeviceContext->RSSetState(nullptr);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::invalidateTextureState() {
    o_assert_dbg(this->d3d11DeviceContext);

    ID3D11ShaderResourceView* const nullVSSRVs[GfxConfig::MaxNumVertexTextures] = { 0 };
    ID3D11ShaderResourceView* const nullFSSRVs[GfxConfig::MaxNumFragmentTextures] = { 0 };
    this->d3d11DeviceContext->VSSetShaderResources(0, GfxConfig::MaxNumVertexTextures, nullVSSRVs);
    this->d3d11DeviceContext->PSSetShaderResources(0, GfxConfig::MaxNumFragmentTextures, nullFSSRVs);

    ID3D11SamplerState* const nullVSSamplers[GfxConfig::MaxNumVertexTextures] = { 0 };
    ID3D11SamplerState* const nullFSSamplers[GfxConfig::MaxNumFragmentTextures] = { 0 };
    this->d3d11DeviceContext->VSSetSamplers(0, GfxConfig::MaxNumVertexTextures, nullVSSamplers);
    this->d3d11DeviceContext->PSSetSamplers(0, GfxConfig::MaxNumFragmentTextures, nullFSSamplers);

    this->d3d11CurVSSRVs.Fill(nullptr);
    this->d3d11CurPSSRVs.Fill(nullptr);
    this->d3d11CurVSSamplers.Fill(nullptr);
    this->d3d11CurPSSamplers.Fill(nullptr);
}

} // namespace _priv
} // namespace Oryol
