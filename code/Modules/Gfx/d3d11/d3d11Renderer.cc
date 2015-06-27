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
rtValid(false),
curRenderTarget(nullptr),
curDrawState(nullptr),
d3d11CurRenderTargetView(nullptr),
d3d11CurDepthStencilView(nullptr),
d3d11CurRasterizerState(nullptr),
d3d11CurDepthStencilState(nullptr),
d3d11CurBlendState(nullptr),
d3d11CurIndexBuffer(nullptr),
d3d11CurInputLayout(nullptr),
d3d11CurVertexShader(nullptr),
d3d11CurPixelShader(nullptr),
curStencilRef(0xFFFF),
curPrimitiveTopology(PrimitiveType::InvalidPrimitiveType) {
    this->d3d11CurVSConstantBuffers.Fill(nullptr);
    this->d3d11CurPSConstantBuffers.Fill(nullptr);
    this->d3d11CurVertexBuffers.Fill(nullptr);
    this->d3d11CurVSShaderResourceViews.Fill(nullptr);
    this->d3d11CurPSShaderResourceViews.Fill(nullptr);
    this->d3d11CurVSSamplerStates.Fill(nullptr);
    this->d3d11CurPSSamplerStates.Fill(nullptr);
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
    this->d3d11CurVSConstantBuffers.Fill(nullptr);
    this->d3d11CurPSConstantBuffers.Fill(nullptr);
    this->d3d11CurVertexBuffers.Fill(nullptr);
    this->d3d11CurVSShaderResourceViews.Fill(nullptr);
    this->d3d11CurPSShaderResourceViews.Fill(nullptr);
    this->d3d11CurVSSamplerStates.Fill(nullptr);
    this->d3d11CurPSSamplerStates.Fill(nullptr);
    this->curVertexStrides.Fill(0);
    this->curVertexOffsets.Fill(0);

    this->curRenderTarget = nullptr;
    this->curDrawState = nullptr;

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
    this->d3d11CurVSConstantBuffers.Fill(nullptr);
    this->d3d11CurPSConstantBuffers.Fill(nullptr);
    this->d3d11CurVertexBuffers.Fill(nullptr);
    this->d3d11CurVSShaderResourceViews.Fill(nullptr);
    this->d3d11CurPSShaderResourceViews.Fill(nullptr);
    this->d3d11CurVSSamplerStates.Fill(nullptr);
    this->d3d11CurPSSamplerStates.Fill(nullptr);
    this->curVertexStrides.Fill(0);
    this->curVertexOffsets.Fill(0);
    this->curStencilRef = 0xFFFF;
    this->curBlendColor = glm::vec4(0.0f);
    this->curPrimitiveTopology = PrimitiveType::InvalidPrimitiveType;
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
    o_assert_dbg(this->d3d11DeviceContext);

    this->invalidateTextureState();
    if (nullptr == rt) {
        this->rtAttrs = this->pointers.displayMgr->GetDisplayAttrs();
        this->d3d11CurRenderTargetView = this->pointers.displayMgr->d3d11RenderTargetView;
        this->d3d11CurDepthStencilView = this->pointers.displayMgr->d3d11DepthStencilView;
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

        this->d3d11CurRenderTargetView = rt->d3d11RenderTargetView;
        this->d3d11CurDepthStencilView = rt->d3d11DepthStencilView;
    }

    this->curRenderTarget = rt;
    this->rtValid = true;
    
    // actually set the render targets in the d3d11 device context
    this->d3d11DeviceContext->OMSetRenderTargets(1, &this->d3d11CurRenderTargetView, this->d3d11CurDepthStencilView);

    // set viewport to cover whole screen
    this->applyViewPort(0, 0, this->rtAttrs.FramebufferWidth, this->rtAttrs.FramebufferHeight, true);
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
d3d11Renderer::applyDrawState(drawState* ds) {
    o_assert_dbg(this->d3d11DeviceContext);

    if (nullptr == ds) {
        // the drawstate is still pending, invalidate rendering
        this->curDrawState = nullptr;
    }
    else {
        o_assert_dbg(ds->d3d11DepthStencilState);
        o_assert_dbg(ds->d3d11RasterizerState);
        o_assert_dbg(ds->d3d11BlendState);

        this->curDrawState = ds;
        o_assert_dbg(ds->prog);
        ds->prog->select(ds->Setup.ProgramSelectionMask);

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
        const int32 vbNumSlots = ds->d3d11IAVertexBuffers.Size();
        o_assert_dbg(vbNumSlots == this->d3d11CurVertexBuffers.Size());
        for (int vbSlotIndex = 0; vbSlotIndex < vbNumSlots; vbSlotIndex++) {
            if (this->d3d11CurVertexBuffers[vbSlotIndex] != ds->d3d11IAVertexBuffers[vbSlotIndex]) {
                this->d3d11CurVertexBuffers[vbSlotIndex] = ds->d3d11IAVertexBuffers[vbSlotIndex];
                vbDirty = true;
            }
            if (this->curVertexStrides[vbSlotIndex] != ds->d3d11IAStrides[vbSlotIndex]) {
                this->curVertexStrides[vbSlotIndex] = ds->d3d11IAStrides[vbSlotIndex];
                vbDirty = true;
            }
            if (this->curVertexOffsets[vbSlotIndex] != ds->d3d11IAOffsets[vbSlotIndex]) {
                this->curVertexOffsets[vbSlotIndex] = ds->d3d11IAOffsets[vbSlotIndex];
                vbDirty = true;
            }
        }
        if (vbDirty) {
            this->d3d11DeviceContext->IASetVertexBuffers(
                0,                                      // StartSlot 
                vbNumSlots,                             // NumBuffers
                &(this->d3d11CurVertexBuffers[0]),      // ppVertexBuffers
                &(this->curVertexStrides[0]),           // pStrides
                &(this->curVertexOffsets[0]));          // pOffsets
        }

        // apply optional index buffer
        if (this->d3d11CurIndexBuffer != ds->meshes[0]->d3d11IndexBuffer) {
            this->d3d11CurIndexBuffer = ds->meshes[0]->d3d11IndexBuffer;
            DXGI_FORMAT d3d11IndexFormat = (DXGI_FORMAT)ds->meshes[0]->indexBufferAttrs.Type;
            this->d3d11DeviceContext->IASetIndexBuffer(ds->meshes[0]->d3d11IndexBuffer, d3d11IndexFormat, 0);
        }

        // apply input layout
        const uint32 selIndex = ds->prog->getSelectionIndex();
        o_assert_dbg(ds->d3d11InputLayouts[selIndex]);
        if (this->d3d11CurInputLayout != ds->d3d11InputLayouts[selIndex]) {
            this->d3d11CurInputLayout = ds->d3d11InputLayouts[selIndex];
            this->d3d11DeviceContext->IASetInputLayout(ds->d3d11InputLayouts[selIndex]);
        }

        // apply shaders
        ID3D11VertexShader* d3d11VS = ds->prog->getSelectedVertexShader();
        if (this->d3d11CurVertexShader != d3d11VS) {
            this->d3d11CurVertexShader = d3d11VS;
            this->d3d11DeviceContext->VSSetShader(d3d11VS, NULL, 0);
        }
        ID3D11PixelShader* d3d11PS = ds->prog->getSelectedPixelShader();
        if (this->d3d11CurPixelShader != d3d11PS) {
            this->d3d11CurPixelShader = d3d11PS;
            this->d3d11DeviceContext->PSSetShader(d3d11PS, NULL, 0);
        }
        
        // apply constant buffers
        ShaderType::Code cbStage = ShaderType::InvalidShaderType;
        int32 cbBindSlot = 0;
        const int numConstantBuffers = ds->prog->getNumUniformBlockEntries();
        o_assert_dbg(numConstantBuffers < ProgramBundleSetup::MaxNumUniformBlocks);
        for (int cbIndex = 0; cbIndex < numConstantBuffers; cbIndex++) {
            ID3D11Buffer* cb = ds->prog->getUniformBlockEntryAt(cbIndex, cbStage, cbBindSlot);
            if (cb) {
                o_assert_dbg(cbBindSlot != InvalidIndex);
                if (ShaderType::VertexShader == cbStage) {
                    if (this->d3d11CurVSConstantBuffers[cbBindSlot] != cb) {
                        this->d3d11CurVSConstantBuffers[cbBindSlot] = cb;
                        this->d3d11DeviceContext->VSSetConstantBuffers(cbBindSlot, 1, &cb);
                    }
                }
                else {
                    if (this->d3d11CurPSConstantBuffers[cbBindSlot] != cb) {
                        this->d3d11CurPSConstantBuffers[cbBindSlot] = cb;
                        this->d3d11DeviceContext->PSSetConstantBuffers(cbBindSlot, 1, &cb);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyUniformBlock(int32 blockIndex, int64 layoutHash, const uint8* ptr, int32 byteSize) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(0 != layoutHash);
    if (nullptr == this->curDrawState) {
        // currently no valid draw state set
        return;
    }

    // get the uniform-layout object for this uniform block
    const programBundle* prog = this->curDrawState->prog;
    o_assert_dbg(prog);
    const UniformLayout& layout = prog->Setup.UniformBlockLayout(blockIndex);

    // check whether the provided struct is type-compatible with the
    // expected uniform block layout
    o_assert2(layout.TypeHash == layoutHash, "incompatible uniform block!\n");
    
    // get the constant buffer entry from the program bundle
    ShaderType::Code cbStage = ShaderType::InvalidShaderType;
    int32 cbBindSlot = 0;
    ID3D11Buffer* cb = prog->getUniformBlockEntryAt(blockIndex, cbStage, cbBindSlot);

    // set textures and samplers
    const uint8* cbufferPtr = nullptr;
    const int numComps = layout.NumComponents();
    int32 shaderSlotIndex = 0;
    for (int compIndex = 0; compIndex < numComps; compIndex++) {
        const auto& comp = layout.ComponentAt(compIndex);
        if (comp.Type == UniformType::Texture) {
            const uint8* valuePtr = ptr + layout.ComponentByteOffset(compIndex);
            const Id& resId = *(const Id*)valuePtr;
            const texture* tex = this->pointers.texturePool->Lookup(resId);
            o_assert_dbg(tex);
            o_assert_dbg(tex->d3d11ShaderResourceView);
            o_assert_dbg(tex->d3d11SamplerState);
            if (ShaderType::VertexShader == cbStage) {
                if (tex->d3d11ShaderResourceView != this->d3d11CurVSShaderResourceViews[shaderSlotIndex]) {
                    this->d3d11CurVSShaderResourceViews[shaderSlotIndex] = tex->d3d11ShaderResourceView;
                    this->d3d11DeviceContext->VSSetShaderResources(shaderSlotIndex, 1, &tex->d3d11ShaderResourceView);
                }
                if (tex->d3d11SamplerState != this->d3d11CurVSSamplerStates[shaderSlotIndex]) {
                    this->d3d11CurVSSamplerStates[shaderSlotIndex] = tex->d3d11SamplerState;
                    this->d3d11DeviceContext->VSSetSamplers(shaderSlotIndex, 1, &tex->d3d11SamplerState);
                }
            }
            else {
                if (tex->d3d11ShaderResourceView != this->d3d11CurPSShaderResourceViews[shaderSlotIndex]) {
                    this->d3d11CurPSShaderResourceViews[shaderSlotIndex] = tex->d3d11ShaderResourceView;
                    this->d3d11DeviceContext->PSSetShaderResources(shaderSlotIndex, 1, &tex->d3d11ShaderResourceView);
                }
                if (tex->d3d11SamplerState != this->d3d11CurPSSamplerStates[shaderSlotIndex]) {
                    this->d3d11CurPSSamplerStates[shaderSlotIndex] = tex->d3d11SamplerState;
                    this->d3d11DeviceContext->PSSetSamplers(shaderSlotIndex, 1, &tex->d3d11SamplerState);
                }
            }
            shaderSlotIndex++;
        }
        else {
            // found the start of the cbuffer struct
            cbufferPtr = ptr + layout.ComponentByteOffset(compIndex);
            break;
        }
    }

    // apply optional uniform block
    // NOTE: UpdateSubresource() and map-discard are equivalent (at least on nvidia)
    if (cb) {
        o_assert_dbg(cbufferPtr);
        this->d3d11DeviceContext->UpdateSubresource(cb, 0, nullptr, cbufferPtr, 0, 0);
    }
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::clear(ClearTarget::Mask clearMask, const glm::vec4& color, float32 depth, uint8 stencil) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert2_dbg((clearMask & ClearTarget::All) != 0, "No clear flags set (note that this has changed from PixelChannel)\n");

    if (clearMask & ClearTarget::Color) {
        o_assert_dbg(this->d3d11CurRenderTargetView);
        this->d3d11DeviceContext->ClearRenderTargetView(this->d3d11CurRenderTargetView, glm::value_ptr(color));
    }
    if (clearMask & (ClearTarget::Depth|ClearTarget::Stencil)) {
        o_assert_dbg(this->d3d11CurDepthStencilView);
        UINT d3d11ClearFlags = 0;
        if (ClearTarget::Depth & clearMask) {
            d3d11ClearFlags |= D3D11_CLEAR_DEPTH;
        }
        if (ClearTarget::Stencil & clearMask) {
            d3d11ClearFlags |= D3D11_CLEAR_STENCIL;
        }
        this->d3d11DeviceContext->ClearDepthStencilView(this->d3d11CurDepthStencilView, d3d11ClearFlags, depth, stencil);
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
    o_assert_dbg(this->curDrawState->meshes[0]);
    if (primGroup.PrimType != this->curPrimitiveTopology) {
        this->curPrimitiveTopology = primGroup.PrimType;
        this->d3d11DeviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)primGroup.PrimType);
    }
    const IndexType::Code indexType = this->curDrawState->meshes[0]->indexBufferAttrs.Type;
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

//------------------------------------------------------------------------------
void
d3d11Renderer::drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_assert_dbg(this->valid);
    o_assert2_dbg(this->rtValid, "No render target set!");
    if (nullptr == this->curDrawState) {
        return;
    }
    o_assert_dbg(this->curDrawState->meshes[0]);
    if (primGroup.PrimType != this->curPrimitiveTopology) {
        this->curPrimitiveTopology = primGroup.PrimType;
        this->d3d11DeviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)primGroup.PrimType);
    }
    const IndexType::Code indexType = this->curDrawState->meshes[0]->indexBufferAttrs.Type;
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
    o_assert_dbg(this->curDrawState->meshes[0]);
    if (primGroupIndex >= this->curDrawState->meshes[0]->numPrimGroups) {
        // this may happen if trying to render a placeholder which doesn't
        // have as many materials as the original mesh, anyway, this isn't
        // a serious error
        return;
    }
    const PrimitiveGroup& primGroup = this->curDrawState->meshes[0]->primGroups[primGroupIndex];
    this->drawInstanced(primGroup, numInstances);
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::updateVertices(mesh* msh, const void* data, int32 numBytes) {
    o_assert_dbg(this->d3d11DeviceContext);
    o_assert_dbg(nullptr != msh);
    o_assert_dbg(msh->d3d11VertexBuffer);
    o_assert_dbg(numBytes > 0);

    const VertexBufferAttrs& attrs = msh->vertexBufferAttrs;
    const Usage::Code vbUsage = attrs.BufferUsage;
    o_assert_dbg((numBytes > 0) && (numBytes <= attrs.ByteSize()));
    o_assert_dbg((vbUsage == Usage::Stream) || (vbUsage == Usage::Dynamic) || (vbUsage == Usage::Static));
    
    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = this->d3d11DeviceContext->Map(msh->d3d11VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    o_assert_dbg(SUCCEEDED(hr));
    std::memcpy(mapped.pData, data, numBytes);
    this->d3d11DeviceContext->Unmap(msh->d3d11VertexBuffer, 0);
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::readPixels(displayMgr* displayManager, void* buf, int32 bufNumBytes) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11Renderer::invalidateMeshState() {
    o_assert_dbg(this->d3d11DeviceContext);

    Log::Info("d3d11Renderer::invalidateMeshState()\n");

    this->d3d11CurIndexBuffer = nullptr;
    this->d3d11CurInputLayout = nullptr;
    this->d3d11CurVertexBuffers.Fill(nullptr);
    this->curVertexStrides.Fill(0);
    this->curVertexOffsets.Fill(0);
    this->curPrimitiveTopology = PrimitiveType::InvalidPrimitiveType;
    this->d3d11DeviceContext->IASetInputLayout(nullptr);
    this->d3d11DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    this->d3d11DeviceContext->IASetVertexBuffers(
        0, 
        this->d3d11CurVertexBuffers.Size(),
        &(this->d3d11CurVertexBuffers[0]),
        &(this->curVertexStrides[0]),
        &(this->curVertexOffsets[0]));
}

//------------------------------------------------------------------------------
void
d3d11Renderer::invalidateProgramState() {
    o_assert_dbg(this->d3d11DeviceContext);

    Log::Info("d3d11Renderer::invalidateProgramState()\n");

    this->d3d11CurVertexShader = nullptr;
    this->d3d11CurPixelShader = nullptr;
    this->d3d11CurVSConstantBuffers.Fill(nullptr);
    this->d3d11CurPSConstantBuffers.Fill(nullptr);
    this->d3d11DeviceContext->VSSetShader(nullptr, nullptr, 0);
    this->d3d11DeviceContext->PSSetShader(nullptr, nullptr, 0);
    this->d3d11DeviceContext->VSSetConstantBuffers(0, ProgramBundleSetup::MaxNumUniformBlocks, &this->d3d11CurVSConstantBuffers[0]);
    this->d3d11DeviceContext->PSSetConstantBuffers(0, ProgramBundleSetup::MaxNumUniformBlocks, &this->d3d11CurPSConstantBuffers[0]);
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

    ID3D11ShaderResourceView* const nullViews[UniformLayout::MaxNumComponents] = { 0 };
    this->d3d11DeviceContext->VSSetShaderResources(0, UniformLayout::MaxNumComponents, nullViews);
    this->d3d11DeviceContext->PSSetShaderResources(0, UniformLayout::MaxNumComponents, nullViews);

    ID3D11SamplerState* const nullSamplerStates[UniformLayout::MaxNumComponents] = { 0 };
    this->d3d11DeviceContext->VSSetSamplers(0, UniformLayout::MaxNumComponents, nullSamplerStates);
    this->d3d11DeviceContext->PSSetSamplers(0, UniformLayout::MaxNumComponents, nullSamplerStates);

    this->d3d11CurVSShaderResourceViews.Fill(nullptr);
    this->d3d11CurPSShaderResourceViews.Fill(nullptr);
    this->d3d11CurVSSamplerStates.Fill(nullptr);
    this->d3d11CurPSSamplerStates.Fill(nullptr);
}

} // namespace _priv
} // namespace Oryol
