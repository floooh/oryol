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
rtValid(false),
frameIndex(0),
curRenderTarget(nullptr),
curDrawState(nullptr),
curPrimaryMesh(nullptr),
d3d11CurRenderTargetView(nullptr),
d3d11CurDepthStencilView(nullptr),
d3d11CurRasterizerState(nullptr),
d3d11CurDepthStencilState(nullptr),
d3d11CurBlendState(nullptr),
d3d11CurIndexBuffer(nullptr),
d3d11CurInputLayout(nullptr),
d3d11CurVertexShader(nullptr),
d3d11CurPixelShader(nullptr),
d3d11CurPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED),
curStencilRef(0xFFFF) {
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
    this->pointers = ptrs;
    this->d3d11Device = this->pointers.displayMgr->d3d11Device;
    this->d3d11DeviceContext = this->pointers.displayMgr->d3d11DeviceContext;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::discard() {
    o_assert_dbg(this->valid);

    this->d3d11CurRenderTargetView = nullptr;
    this->d3d11CurDepthStencilView = nullptr;
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

    this->curRenderTarget = nullptr;
    this->curDrawState = nullptr;
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
    this->curRenderTarget = nullptr;
    this->curDrawState = nullptr;
    this->d3d11CurRenderTargetView = nullptr;
    this->d3d11CurDepthStencilView = nullptr;
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
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
void
d3d11Renderer::commitFrame() {
    o_assert_dbg(this->valid);
    this->rtValid = false;
    this->curRenderTarget = nullptr;
    this->curDrawState = nullptr;
    this->curPrimaryMesh = nullptr;
    this->frameIndex++;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
d3d11Renderer::renderTargetAttrs() const {
    return this->rtAttrs;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyRenderTarget(texture* rt, const ClearState& clearState) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->d3d11DeviceContext);

    this->invalidateTextureState();
    if (nullptr == rt) {
        this->rtAttrs = this->pointers.displayMgr->GetDisplayAttrs();
        this->d3d11CurRenderTargetView = this->pointers.displayMgr->d3d11RenderTargetView;
        this->d3d11CurDepthStencilView = this->pointers.displayMgr->d3d11DepthStencilView;
    }
    else {
        this->rtAttrs = DisplayAttrs::FromTextureAttrs(rt->textureAttrs);
        this->d3d11CurRenderTargetView = rt->d3d11RenderTargetView;
        this->d3d11CurDepthStencilView = rt->d3d11DepthStencilView;
    }

    this->curRenderTarget = rt;
    this->rtValid = true;
    
    // actually set the render targets in the d3d11 device context
    this->d3d11DeviceContext->OMSetRenderTargets(1, &this->d3d11CurRenderTargetView, this->d3d11CurDepthStencilView);

    // set viewport to cover whole screen
    this->applyViewPort(0, 0, this->rtAttrs.FramebufferWidth, this->rtAttrs.FramebufferHeight, true);

    // perform clear action
    if (clearState.Actions & ClearState::ColorBit) {
        if (this->d3d11CurRenderTargetView) {
            const FLOAT* clearColor = glm::value_ptr(clearState.Color);
            this->d3d11DeviceContext->ClearRenderTargetView(this->d3d11CurRenderTargetView, clearColor);
        }
    }
    if (clearState.Actions & ClearState::DepthStencilBits) {
        if (this->d3d11CurDepthStencilView) {
            UINT d3d11ClearFlags = 0;
            if (clearState.Actions & ClearState::DepthBit) {
                d3d11ClearFlags |= D3D11_CLEAR_DEPTH;
            }
            if (clearState.Actions & ClearState::StencilBit) {
                d3d11ClearFlags |= D3D11_CLEAR_STENCIL;
            }
            const FLOAT d = clearState.Depth;
            const UINT8 s = clearState.Stencil;
            this->d3d11DeviceContext->ClearDepthStencilView(this->d3d11CurDepthStencilView, d3d11ClearFlags, d, s);
        }
    }
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::applyViewPort(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_assert_dbg(this->d3d11DeviceContext);

    D3D11_VIEWPORT vp;
    vp.TopLeftX = (FLOAT) x;
    vp.TopLeftY = (FLOAT) (originTopLeft ? y : (this->rtAttrs.FramebufferHeight - (y + height)));
    vp.Width    = (FLOAT) width;
    vp.Height   = (FLOAT) height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    this->d3d11DeviceContext->RSSetViewports(1, &vp);
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::applyScissorRect(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_assert_dbg(this->d3d11DeviceContext);

    D3D11_RECT rect;
    rect.left = x;
    rect.top = originTopLeft ? y : this->rtAttrs.FramebufferHeight - (y + height);
    rect.right = x + width;
    rect.bottom = originTopLeft ? (y + height) : (this->rtAttrs.FramebufferHeight - y);
    this->d3d11DeviceContext->RSSetScissorRects(1, &rect);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyDrawState(drawState* ds, mesh** meshes, int numMeshes) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(ds);
    o_assert_dbg(ds->shd);
    o_assert_dbg(meshes && numMeshes > 0);

    // if any of the meshes are still loading, cancel the next draw state
    for (int i = 0; i < numMeshes; i++) {
        if (nullptr == meshes[i]) {
            this->curDrawState = nullptr;
            return;
        }
    }
    o_assert_dbg(ds->d3d11DepthStencilState);
    o_assert_dbg(ds->d3d11RasterizerState);
    o_assert_dbg(ds->d3d11BlendState);
    o_assert2(ds->Setup.BlendState.ColorFormat == this->rtAttrs.ColorPixelFormat, "ColorFormat in BlendState must match current render target!\n");
    o_assert2(ds->Setup.BlendState.DepthFormat == this->rtAttrs.DepthPixelFormat, "DepthFormat in BlendState must match current render target!\n");
    o_assert2(ds->Setup.RasterizerState.SampleCount == this->rtAttrs.SampleCount, "SampleCount in RasterizerState must match current render target!\n");

    this->curDrawState = ds;
    this->curPrimaryMesh = meshes[0];
    o_assert_dbg(this->curPrimaryMesh);

    // apply state objects (if state has changed)
    if (ds->d3d11RasterizerState != this->d3d11CurRasterizerState) {
        this->d3d11CurRasterizerState = ds->d3d11RasterizerState;
        this->d3d11DeviceContext->RSSetState(ds->d3d11RasterizerState);
    }
    if ((ds->d3d11DepthStencilState != this->d3d11CurDepthStencilState) ||
        (ds->Setup.DepthStencilState.StencilRef != this->curStencilRef)) {

        this->d3d11CurDepthStencilState = ds->d3d11DepthStencilState;
        this->curStencilRef = ds->Setup.DepthStencilState.StencilRef;
        this->d3d11DeviceContext->OMSetDepthStencilState(ds->d3d11DepthStencilState, ds->Setup.DepthStencilState.StencilRef);
    }
    if ((ds->d3d11BlendState != this->d3d11CurBlendState) ||
        glm::any(glm::notEqual(ds->Setup.BlendColor, this->curBlendColor))) {
        
        this->d3d11CurBlendState = ds->d3d11BlendState;
        this->curBlendColor = ds->Setup.BlendColor;
        this->d3d11DeviceContext->OMSetBlendState(ds->d3d11BlendState, glm::value_ptr(ds->Setup.BlendColor), 0xFFFFFFFF);
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
    if (this->d3d11CurPrimitiveTopology != ds->d3d11PrimTopology) {
        this->d3d11CurPrimitiveTopology = ds->d3d11PrimTopology;
        this->d3d11DeviceContext->IASetPrimitiveTopology(ds->d3d11PrimTopology);
    }

    // apply optional index buffer (can be nullptr!)
    if (this->d3d11CurIndexBuffer != this->curPrimaryMesh->d3d11IndexBuffer) {
        this->d3d11CurIndexBuffer = this->curPrimaryMesh->d3d11IndexBuffer;
        DXGI_FORMAT d3d11IndexFormat = d3d11Types::asIndexType(this->curPrimaryMesh->indexBufferAttrs.Type);
        this->d3d11DeviceContext->IASetIndexBuffer(this->curPrimaryMesh->d3d11IndexBuffer, d3d11IndexFormat, 0);
    }

    // apply input layout and shaders
    if (this->d3d11CurInputLayout != ds->d3d11InputLayout) {
        this->d3d11CurInputLayout = ds->d3d11InputLayout;
        this->d3d11DeviceContext->IASetInputLayout(ds->d3d11InputLayout);
    }

    // apply shaders
    if (this->d3d11CurVertexShader != ds->d3d11VertexShader) {
        this->d3d11CurVertexShader = ds->d3d11VertexShader;
        this->d3d11DeviceContext->VSSetShader(ds->d3d11VertexShader, NULL, 0);
    }
    if (this->d3d11CurPixelShader != ds->d3d11PixelShader) {
        this->d3d11CurPixelShader = ds->d3d11PixelShader;
        this->d3d11DeviceContext->PSSetShader(ds->d3d11PixelShader, NULL, 0);
    }
        
    // apply vertex-shader-stage constant buffers
    for (int bindSlot = 0; bindSlot < GfxConfig::MaxNumUniformBlocksPerStage; bindSlot++) {
        // NOTE: cb can be nullptr!
        ID3D11Buffer* cb = ds->shd->getConstantBuffer(ShaderStage::VS, bindSlot);
        if (this->d3d11CurVSCBs[bindSlot] != cb) {
            this->d3d11CurVSCBs[bindSlot] = cb;
            this->d3d11DeviceContext->VSSetConstantBuffers(bindSlot, 1, &cb);
        }
    }

    // apply fragment-shader-stage constant buffers
    for (int bindSlot = 0; bindSlot < GfxConfig::MaxNumUniformBlocksPerStage; bindSlot++) {
        // NOTE: cb can be nullptr!
        ID3D11Buffer* cb = ds->shd->getConstantBuffer(ShaderStage::FS, bindSlot);
        if (this->d3d11CurPSCBs[bindSlot] != cb) {
            this->d3d11CurPSCBs[bindSlot] = cb;
            this->d3d11DeviceContext->PSSetConstantBuffers(bindSlot, 1, &cb);
        }
    }
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyUniformBlock(ShaderStage::Code ubBindStage, int32 ubBindSlot, int64 layoutHash, const uint8* ptr, int32 byteSize) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(0 != layoutHash);
    if (nullptr == this->curDrawState) {
        // currently no valid draw state set
        return;
    }

    const shader* shd = this->curDrawState->shd;
    o_assert_dbg(shd);
    
    #if ORYOL_DEBUG
    // verify that the provided uniform block struct is type compatible
    // with the uniform block expected at the binding stage and slot
    int32 ubIndex = shd->Setup.UniformBlockIndexByStageAndSlot(ubBindStage, ubBindSlot);
    const UniformBlockLayout& layout = shd->Setup.UniformBlockLayout(ubIndex);
    o_assert2(layout.TypeHash == layoutHash, "incompatible uniform block!\n");
    #if !ORYOL_WIN32 // NOTE: VS 32-bit sometimes adds useless padding bytes at end of structs
        o_assert_dbg(layout.ByteSize() == byteSize);
    #endif
    #endif

    // NOTE: UpdateSubresource() and map-discard are equivalent (at least on nvidia)
    ID3D11Buffer* cb = shd->getConstantBuffer(ubBindStage, ubBindSlot);
    o_assert_dbg(cb);
    this->d3d11DeviceContext->UpdateSubresource(cb, 0, nullptr, ptr, 0, 0);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyTextureBlock(ShaderStage::Code bindStage, int32 bindSlot, int64 layoutHash, texture** textures, int32 numTextures) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(numTextures <= GfxConfig::MaxNumTexturesPerStage);
    if (nullptr == this->curDrawState) {
        return;
    }

    // if any of the provided texture pointers are not valid, this means one of the
    // textures isn't valid yet, in this case, disable rendering for the next draw call
    for (int i = 0; i < numTextures; i++) {
        if (nullptr == textures[i]) {
            this->curDrawState = nullptr;
            return;
        }
    }

    // check if the provided texture types are compatible
    #if ORYOL_DEBUG
    const shader* shd = this->curDrawState->shd;
    o_assert_dbg(shd);
    int32 texBlockIndex = shd->Setup.TextureBlockIndexByStageAndSlot(bindStage, bindSlot);
    o_assert_dbg(InvalidIndex != texBlockIndex);
    const TextureBlockLayout& layout = shd->Setup.TextureBlockLayout(texBlockIndex);
    o_assert2(layout.TypeHash == layoutHash, "incompatible texture block!\n");
    for (int i = 0; i < numTextures; i++) {
        const auto& texBlockComp = layout.ComponentAt(layout.ComponentIndexForBindSlot(i));
        if (texBlockComp.Type != textures[i]->textureAttrs.Type) {
            o_error("Texture type mismatch at slot '%s'\n", texBlockComp.Name.AsCStr());
        }
    }
    #endif

    // apply textures and samplers
    if (ShaderStage::VS == bindStage) {
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
d3d11Renderer::draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert2_dbg(this->rtValid, "No render target set!\n");
    if (nullptr == this->curDrawState) {
        return;
    }
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    const IndexType::Code indexType = msh->indexBufferAttrs.Type;
    if (indexType != IndexType::None) {
        this->d3d11DeviceContext->DrawIndexed(primGroup.NumElements, primGroup.BaseElement, 0);
    }
    else {
        this->d3d11DeviceContext->Draw(primGroup.NumElements, primGroup.BaseElement);        
    }
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::draw(int32 primGroupIndex) {
    o_assert_dbg(this->valid);
    if (nullptr == this->curDrawState) {
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
    this->draw(primGroup);
}

//------------------------------------------------------------------------------
void
d3d11Renderer::drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");
    if (nullptr == this->curDrawState) {
        return;
    }
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    const IndexType::Code indexType = msh->indexBufferAttrs.Type;
    if (indexType != IndexType::None) {
        this->d3d11DeviceContext->DrawIndexedInstanced(primGroup.NumElements, numInstances, primGroup.BaseElement, 0, 0);
    }
    else {
        this->d3d11DeviceContext->DrawInstanced(primGroup.NumElements, numInstances, 0, 0);
    }
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::drawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_assert_dbg(this->valid);
    if (nullptr == this->curDrawState) {
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
    this->drawInstanced(primGroup, numInstances);
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::updateVertices(mesh* msh, const void* data, int32 numBytes) {
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
d3d11Renderer::updateIndices(mesh* msh, const void* data, int32 numBytes) {
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
    for (int32 mipIndex = 0; mipIndex < attrs.NumMipMaps; mipIndex++) {
        o_assert_dbg(offsetsAndSizes.Sizes[0][mipIndex] > 0);
        const int32 mipWidth = std::max(attrs.Width >> mipIndex, 1);
        const int32 mipHeight = std::max(attrs.Height >> mipIndex, 1);
        const int32 srcPitch = PixelFormat::RowPitch(attrs.ColorFormat, mipWidth);
        HRESULT hr = this->d3d11DeviceContext->Map(
            tex->d3d11Texture2D,        // pResource
            mipIndex,                   // Subresource
            D3D11_MAP_WRITE_DISCARD,    // MapType
            0,                          // MapFlags
            &mapped);                   // pMappedResource
        o_assert_dbg(SUCCEEDED(hr));
        o_assert_dbg(srcPitch <= (int32)mapped.RowPitch);
        uint8* dstPtr = (uint8*)mapped.pData;
        const uint8* srcPtr = ((const uint8*)data) + offsetsAndSizes.Offsets[0][mipIndex];
        if (srcPitch == mapped.RowPitch) {
            const int32 mipSize = offsetsAndSizes.Sizes[0][mipIndex];
            o_assert_dbg(mipSize == (srcPitch*mipHeight));
            std::memcpy(dstPtr, srcPtr, mipSize);
        }
        else {
            o_error("d3dRenderer::updateTexture(): srcPitch!=dstPitch, FIXME UNTESTED!\n");
            for (int32 rowIndex = 0; rowIndex < mipHeight; rowIndex++) {
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
d3d11Renderer::readPixels(void* buf, int32 bufNumBytes) {
    o_error("d3d11Renderer::readPixels() NOT IMPLEMENTED!\n");
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
d3d11Renderer::invalidateDrawState() {
    o_assert_dbg(this->d3d11DeviceContext);

    Log::Info("d3d11Renderer::invalidateDrawState()\n");

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

    ID3D11ShaderResourceView* const nullSRVs[GfxConfig::MaxNumTexturesPerStage] = { 0 };
    this->d3d11DeviceContext->VSSetShaderResources(0, GfxConfig::MaxNumTexturesPerStage, nullSRVs);
    this->d3d11DeviceContext->PSSetShaderResources(0, GfxConfig::MaxNumTexturesPerStage, nullSRVs);

    ID3D11SamplerState* const nullSamplers[GfxConfig::MaxNumTexturesPerStage] = { 0 };
    this->d3d11DeviceContext->VSSetSamplers(0, GfxConfig::MaxNumTexturesPerStage, nullSamplers);
    this->d3d11DeviceContext->PSSetSamplers(0, GfxConfig::MaxNumTexturesPerStage, nullSamplers);

    this->d3d11CurVSSRVs.Fill(nullptr);
    this->d3d11CurPSSRVs.Fill(nullptr);
    this->d3d11CurVSSamplers.Fill(nullptr);
    this->d3d11CurPSSamplers.Fill(nullptr);
}

} // namespace _priv
} // namespace Oryol
