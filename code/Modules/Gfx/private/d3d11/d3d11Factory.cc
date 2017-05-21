//------------------------------------------------------------------------------
//  d3d11Factory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11Factory.h"
#include "d3d11Types.h"
#include "d3d11_impl.h"
#include "Gfx/private/resource.h"
#include "Gfx/private/renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
d3d11Factory::setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->subResourceData);
    gfxFactoryBase::setup(ptrs);
    this->subResourceData = (D3D11_SUBRESOURCE_DATA*) Memory::Alloc(sizeof(D3D11_SUBRESOURCE_DATA) * maxNumSubResourceData);
}

//------------------------------------------------------------------------------
void
d3d11Factory::discard() {
    o_assert_dbg(this->subResourceData);
    Memory::Free(this->subResourceData);
    this->subResourceData = nullptr;
    gfxFactoryBase::discard();
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11Factory::initMesh(mesh& msh, const void* data, int size) {
    o_assert_dbg(this->isValid);
    if (msh.Setup.ShouldSetupFullScreenQuad()) {
        return this->initFullscreenQuad(msh);
    }
    else {
        return this->initStdMesh(msh, data, size);
    }
}

//------------------------------------------------------------------------------
void
d3d11Factory::destroyMesh(mesh& msh) {
    this->pointers.renderer->invalidateMeshState();
    if (msh.d3d11VertexBuffer) {
        msh.d3d11VertexBuffer->Release();
    }
    if (msh.d3d11IndexBuffer) {
        msh.d3d11IndexBuffer->Release();
    }
    msh.Clear();
}

//------------------------------------------------------------------------------
ID3D11Buffer*
d3d11Factory::createBuffer(const void* data, uint32_t dataSize, uint32_t d3d11BindFlags, Usage::Code usage) {
    o_assert_dbg((D3D11_BIND_VERTEX_BUFFER == d3d11BindFlags) || (D3D11_BIND_INDEX_BUFFER == d3d11BindFlags));
    ID3D11Device* d3d11Device = this->pointers.renderer->d3d11Device;
    o_assert_dbg(d3d11Device);

    D3D11_BUFFER_DESC desc = { };
    desc.ByteWidth = dataSize;
    desc.Usage = d3d11Types::asResourceUsage(usage);
    desc.BindFlags = d3d11BindFlags;
    desc.CPUAccessFlags = d3d11Types::asResourceCPUAccessFlag(usage);

    D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;
    D3D11_SUBRESOURCE_DATA initData = { };
    if (data) {
        initData.pSysMem = data;
        initDataPtr = &initData;
    }
    ID3D11Buffer* buf = nullptr;
    HRESULT hr = d3d11Device->CreateBuffer(&desc, initDataPtr, &buf);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(buf);

    return buf;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11Factory::initFullscreenQuad(mesh& mesh) {
    o_assert_dbg(nullptr == mesh.d3d11VertexBuffer);
    o_assert_dbg(nullptr == mesh.d3d11IndexBuffer);

    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = 4;
    vbAttrs.BufferUsage = Usage::Immutable;
    vbAttrs.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
    vbAttrs.Layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    mesh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = 6;
    ibAttrs.Type = IndexType::Index16;
    ibAttrs.BufferUsage = Usage::Immutable;
    mesh.indexBufferAttrs = ibAttrs;

    mesh.numPrimGroups = 1;
    mesh.primGroups[0] = PrimitiveGroup(0, 6);

    // vertices
    const float topV = mesh.Setup.FullScreenQuadFlipV ? 0.0f : 1.0f;
    const float botV = mesh.Setup.FullScreenQuadFlipV ? 1.0f : 0.0f;
    float vertices[] = {
        -1.0f, +1.0f, 0.0f, 0.0f, topV,     // top-left corner
        +1.0f, +1.0f, 0.0f, 1.0f, topV,     // top-right corner
        +1.0f, -1.0f, 0.0f, 1.0f, botV,     // bottom-right corner
        -1.0f, -1.0f, 0.0f, 0.0f, botV      // bottom-left corner
    };

    // indices
    uint16_t indices[] = {
        0, 2, 1,            // topleft -> bottomright -> topright
        0, 3, 2,            // topleft -> bottomleft -> bottomright
    };

    // create vertex and index buffer
    mesh.d3d11VertexBuffer = this->createBuffer(vertices, sizeof(vertices), D3D11_BIND_VERTEX_BUFFER, mesh.vertexBufferAttrs.BufferUsage);
    mesh.d3d11IndexBuffer = this->createBuffer(indices, sizeof(indices), D3D11_BIND_INDEX_BUFFER, mesh.indexBufferAttrs.BufferUsage);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11Factory::initStdMesh(mesh& mesh, const void* data, int size) {
    o_assert_dbg(nullptr == mesh.d3d11VertexBuffer);
    o_assert_dbg(nullptr == mesh.d3d11IndexBuffer);

    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = mesh.Setup.NumVertices;
    vbAttrs.Layout = mesh.Setup.Layout;
    vbAttrs.BufferUsage = mesh.Setup.VertexUsage;
    mesh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = mesh.Setup.NumIndices;
    ibAttrs.Type = mesh.Setup.IndicesType;
    ibAttrs.BufferUsage = mesh.Setup.IndexUsage;
    mesh.indexBufferAttrs = ibAttrs;

    mesh.numPrimGroups = mesh.Setup.NumPrimitiveGroups();
    o_assert_dbg(mesh.numPrimGroups < GfxConfig::MaxNumPrimGroups);
    for (int i = 0; i < mesh.numPrimGroups; i++) {
        mesh.primGroups[i] = mesh.Setup.PrimitiveGroup(i);
    }

    if (mesh.Setup.NumVertices > 0) {
        const int vbSize = vbAttrs.NumVertices * vbAttrs.Layout.ByteSize();
        const uint8_t* vertices = nullptr;
        if (InvalidIndex != mesh.Setup.VertexDataOffset) {
            const uint8_t* ptr = (const uint8_t*)data;
            o_assert_dbg(ptr && (size > 0));
            vertices = ptr + mesh.Setup.VertexDataOffset;
            o_assert_dbg((ptr + size) >= (vertices + vbSize));
        }
        mesh.d3d11VertexBuffer = this->createBuffer(vertices, vbSize, D3D11_BIND_VERTEX_BUFFER, vbAttrs.BufferUsage);
    }
    if (IndexType::None != ibAttrs.Type) {
        const int ibSize = ibAttrs.NumIndices * IndexType::ByteSize(ibAttrs.Type);
        const uint8_t* indices = nullptr;
        if (InvalidIndex != mesh.Setup.IndexDataOffset) {
            const uint8_t* ptr = (const uint8_t*)data;
            o_assert_dbg(ptr && (size > 0));
            indices = ((const uint8_t*)ptr) + mesh.Setup.IndexDataOffset;
            o_assert_dbg((ptr + size) >= (indices + ibSize));
        }
        mesh.d3d11IndexBuffer = this->createBuffer(indices, ibSize, D3D11_BIND_INDEX_BUFFER, ibAttrs.BufferUsage);
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
static ID3D11ShaderResourceView*
createShaderResourceView(ID3D11Device* d3d11Device, const texture& tex, ID3D11Resource* d3d11Tex, DXGI_FORMAT d3d11Format) {
    D3D11_SHADER_RESOURCE_VIEW_DESC shdResViewDesc = { };
    shdResViewDesc.Format = d3d11Format;
    switch (tex.Setup.Type) {
        case TextureType::Texture2D:
            shdResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shdResViewDesc.Texture2D.MostDetailedMip = 0;
            shdResViewDesc.Texture2D.MipLevels = tex.Setup.NumMipMaps;
            break;
        case TextureType::TextureCube:
            shdResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            shdResViewDesc.TextureCube.MostDetailedMip = 0;
            shdResViewDesc.TextureCube.MipLevels = tex.Setup.NumMipMaps;
            break;
        case TextureType::Texture3D:
            shdResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
            shdResViewDesc.Texture3D.MostDetailedMip = 0;
            shdResViewDesc.Texture3D.MipLevels = tex.Setup.NumMipMaps;
            break;
        case TextureType::TextureArray:
            shdResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            shdResViewDesc.Texture2DArray.MostDetailedMip = 0;
            shdResViewDesc.Texture2DArray.MipLevels = tex.Setup.NumMipMaps;
            shdResViewDesc.Texture2DArray.FirstArraySlice = 0;
            shdResViewDesc.Texture2DArray.ArraySize = tex.Setup.Depth;
            break;
        default:
            o_error("d3d11TextureFactory::createShaderResourceView: invalid texture type!\n");
            break;
    }
    ID3D11ShaderResourceView* d3d11ShaderResourceView = nullptr;
    HRESULT hr = d3d11Device->CreateShaderResourceView(d3d11Tex, &shdResViewDesc, &d3d11ShaderResourceView);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != d3d11ShaderResourceView);
    return d3d11ShaderResourceView;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11Factory::initTexture(texture& tex, const void* data, int size) {
    o_assert_dbg(this->subResourceData);
    o_assert_dbg(!tex.d3d11Texture2D);
    o_assert_dbg(!tex.d3d11Texture3D);
    o_assert_dbg(!tex.d3d11ShaderResourceView);
    o_assert_dbg(!tex.d3d11SamplerState);
    o_assert_dbg(!tex.d3d11DepthStencilTexture);
    o_assert_dbg(!tex.d3d11MSAATexture2D);
    ID3D11Device* d3d11Device = this->pointers.renderer->d3d11Device;

    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(!setup.ShouldSetupFromNativeTexture());

    // subresourcedata array if initial data is provided
    D3D11_SUBRESOURCE_DATA* pInitialData = nullptr;
    if (data) {
        const uint8_t* srcPtr = (const uint8_t*)data;
        const int numFaces = setup.Type==TextureType::TextureCube ? 6:1;
        const int numSlices = setup.Type==TextureType::TextureArray ? setup.Depth:1;
        const int sliceSize = size / numSlices;
        o_assert_dbg((numSlices * sliceSize) == size);
        const int numMipMaps = setup.ImageData.NumMipMaps;
        int subResourceDataIndex = 0;
        for (int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
            for (int sliceIndex = 0; sliceIndex < numSlices; sliceIndex++) {
                int sliceOffset = sliceIndex * sliceSize;
                for (int mipIndex = 0; mipIndex < numMipMaps; mipIndex++, subResourceDataIndex++) {
                    o_assert_dbg(subResourceDataIndex < maxNumSubResourceData);
                    D3D11_SUBRESOURCE_DATA& subResData = this->subResourceData[subResourceDataIndex];
                    subResData.pSysMem = srcPtr + sliceOffset + setup.ImageData.Offsets[faceIndex][mipIndex]; 
                    const int mipWidth = std::max(setup.Width >> mipIndex, 1);
                    const int mipHeight = std::max(setup.Height >> mipIndex, 1);
                    subResData.SysMemPitch = PixelFormat::RowPitch(setup.ColorFormat, mipWidth);
                    if (setup.Type == TextureType::Texture3D) {
                        const int mipDepth = std::max(setup.Depth >> mipIndex, 1);
                        subResData.SysMemSlicePitch = PixelFormat::ImagePitch(setup.ColorFormat, mipWidth, mipHeight);
                    }
                    else {
                        subResData.SysMemSlicePitch = 0;
                    }
                }
            }
        }
        pInitialData = this->subResourceData;
    }

    // create the color texture
    HRESULT hr;
    if (setup.Type != TextureType::Texture3D) {
        // setup texture desc for 2D texture
        D3D11_TEXTURE2D_DESC texDesc = { };
        texDesc.Width = setup.Width;
        texDesc.Height = setup.Height;
        texDesc.MipLevels = setup.NumMipMaps;
        if (TextureType::TextureArray == setup.Type) {
            texDesc.ArraySize = setup.Depth;
        }
        else if (TextureType::TextureCube == setup.Type) {
            texDesc.ArraySize = 6;
        }
        else {
            texDesc.ArraySize = 1;
        }
        if (setup.IsRenderTarget) {
            texDesc.Format = tex.d3d11ColorFormat = d3d11Types::asRenderTargetFormat(setup.ColorFormat);
            o_assert2_dbg(texDesc.Format != DXGI_FORMAT_UNKNOWN, "Invalid render target pixel format!");
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
            texDesc.CPUAccessFlags = 0;
        }
        else {
            texDesc.Format = tex.d3d11ColorFormat = d3d11Types::asTextureFormat(setup.ColorFormat);
            if (DXGI_FORMAT_UNKNOWN == texDesc.Format) {
                o_warn("d3d11TextureFactory: texture pixel format not supported in D3D11\n");
                return ResourceState::Failed;
            }
            texDesc.Usage = d3d11Types::asResourceUsage(setup.TextureUsage);
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texDesc.CPUAccessFlags = d3d11Types::asResourceCPUAccessFlag(setup.TextureUsage);
        }
        texDesc.SampleDesc.Count = 1; 
        texDesc.SampleDesc.Quality = 0;
        texDesc.MiscFlags = setup.Type == TextureType::TextureCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
        hr = d3d11Device->CreateTexture2D(&texDesc, pInitialData, &tex.d3d11Texture2D);
        o_assert(SUCCEEDED(hr));
        
        // also create an MSAA texture?
        if (setup.SampleCount > 1) {
            texDesc.SampleDesc.Count = setup.SampleCount;
            texDesc.SampleDesc.Quality = 0;
            hr = d3d11Device->CreateTexture2D(&texDesc, pInitialData, &tex.d3d11MSAATexture2D);
            o_assert(SUCCEEDED(hr));
        }
        
        // shader-resource-view (always on the non-MSAA texture)
        tex.d3d11ShaderResourceView = createShaderResourceView(d3d11Device, tex, tex.d3d11Texture2D, texDesc.Format);
        o_assert(tex.d3d11ShaderResourceView);
    }
    else {
        // 3D texture
        D3D11_TEXTURE3D_DESC texDesc = { };
        texDesc.Width = setup.Width;
        texDesc.Height = setup.Height;
        texDesc.Depth = setup.Depth;
        texDesc.MipLevels = setup.NumMipMaps;
        if (setup.IsRenderTarget) {
            texDesc.Format = tex.d3d11ColorFormat = d3d11Types::asRenderTargetFormat(setup.ColorFormat);
            o_assert2_dbg(texDesc.Format != DXGI_FORMAT_UNKNOWN, "Invalid render target color pixel format!");
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            texDesc.CPUAccessFlags = 0;
        }
        else {
            texDesc.Format = tex.d3d11ColorFormat = d3d11Types::asTextureFormat(setup.ColorFormat);
            o_assert2_dbg(texDesc.Format != DXGI_FORMAT_UNKNOWN, "Invalid texture pixel format!");
            texDesc.Usage = d3d11Types::asResourceUsage(setup.TextureUsage);
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texDesc.CPUAccessFlags = d3d11Types::asResourceCPUAccessFlag(setup.TextureUsage);
        }
        texDesc.MiscFlags = 0;
        hr = d3d11Device->CreateTexture3D(&texDesc, pInitialData, &tex.d3d11Texture3D);
        o_assert(SUCCEEDED(hr));
        tex.d3d11ShaderResourceView = createShaderResourceView(d3d11Device, tex, tex.d3d11Texture3D, texDesc.Format);
        o_assert(tex.d3d11ShaderResourceView);
    }

    // optional depth-stencil-buffer texture
    if (setup.IsRenderTarget && (setup.DepthFormat != PixelFormat::InvalidPixelFormat)) {
        // create depth-buffer-texture
        D3D11_TEXTURE2D_DESC dsDesc = {};
        dsDesc.Width = setup.Width;
        dsDesc.Height = setup.Height;
        dsDesc.MipLevels = setup.NumMipMaps;
        dsDesc.ArraySize = 1;
        dsDesc.Format = d3d11Types::asRenderTargetFormat(setup.DepthFormat);
        o_assert2_dbg(dsDesc.Format != DXGI_FORMAT_UNKNOWN, "Invalid render target depth pixel format!");
        dsDesc.Usage = D3D11_USAGE_DEFAULT;
        dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        dsDesc.CPUAccessFlags = 0;
        dsDesc.SampleDesc.Count = setup.SampleCount;
        dsDesc.SampleDesc.Quality = 0;
        dsDesc.MiscFlags = 0;
        hr = d3d11Device->CreateTexture2D(&dsDesc, nullptr, &tex.d3d11DepthStencilTexture);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(nullptr != tex.d3d11DepthStencilTexture);
    }

    // create sampler object
    D3D11_SAMPLER_DESC smpDesc = { };
    smpDesc.Filter = d3d11Types::asSamplerFilter(tex.Setup.Sampler.MagFilter, tex.Setup.Sampler.MinFilter);
    smpDesc.AddressU = d3d11Types::asTextureAddressMode(tex.Setup.Sampler.WrapU);
    smpDesc.AddressV = d3d11Types::asTextureAddressMode(tex.Setup.Sampler.WrapV);
    smpDesc.AddressW = d3d11Types::asTextureAddressMode(tex.Setup.Sampler.WrapW);
    smpDesc.MipLODBias = 0.0f;
    smpDesc.MaxAnisotropy = 1;
    smpDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    smpDesc.BorderColor[0] = 1.0f;
    smpDesc.BorderColor[1] = 1.0f;
    smpDesc.BorderColor[2] = 1.0f;
    smpDesc.BorderColor[3] = 1.0f;
    smpDesc.MinLOD = -D3D11_FLOAT32_MAX;
    smpDesc.MaxLOD = D3D11_FLOAT32_MAX;
    ID3D11SamplerState* d3d11SamplerState = nullptr;
    hr = d3d11Device->CreateSamplerState(&smpDesc, &tex.d3d11SamplerState);
    o_assert(SUCCEEDED(hr));
    o_assert(tex.d3d11SamplerState);

    // fill texture attributes
    o_assert_dbg(DXGI_FORMAT_UNKNOWN != tex.d3d11ColorFormat);
    TextureAttrs attrs;
    attrs.Locator = tex.Setup.Locator;
    attrs.Type = tex.Setup.Type;
    attrs.ColorFormat = tex.Setup.ColorFormat;
    attrs.DepthFormat = tex.Setup.DepthFormat;
    attrs.SampleCount = tex.Setup.SampleCount;
    attrs.TextureUsage = tex.Setup.TextureUsage;
    attrs.Width = tex.Setup.Width;
    attrs.Height = tex.Setup.Height;
    attrs.Depth = tex.Setup.Depth;
    attrs.NumMipMaps = tex.Setup.NumMipMaps;
    attrs.IsRenderTarget = tex.Setup.IsRenderTarget;
    attrs.HasDepthBuffer = tex.Setup.HasDepth();
    tex.textureAttrs = attrs;

    // and done
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11Factory::destroyTexture(texture& tex) {
    o_assert_dbg(this->isValid);
    this->pointers.renderer->invalidateTextureState();
    if (tex.d3d11Texture2D) {
        tex.d3d11Texture2D->Release();
    }
    if (tex.d3d11Texture3D) {
        tex.d3d11Texture3D->Release();
    }
    if (tex.d3d11ShaderResourceView) {
        tex.d3d11ShaderResourceView->Release();
    }
    if (tex.d3d11SamplerState) {
        tex.d3d11SamplerState->Release();
    }
    if (tex.d3d11DepthStencilTexture) {
        tex.d3d11DepthStencilTexture->Release();
    }
    if (tex.d3d11MSAATexture2D) {
        tex.d3d11MSAATexture2D->Release();
    }
    tex.Clear();
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11Factory::initRenderPass(renderPass& rp) {
    o_assert_dbg(this->isValid);
    ID3D11Device* d3d11Device = this->pointers.renderer->d3d11Device;

    gfxFactoryBase::initRenderPass(rp);
    o_assert_dbg(rp.colorTextures[0]);
    const bool isMSAA = rp.colorTextures[0]->Setup.SampleCount > 1;

    // create render-target-view objects
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        o_assert_dbg(nullptr == rp.d3d11RenderTargetViews[i]);
        texture* colorTex = rp.colorTextures[i];
        if (colorTex) {
            D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = { };
            ID3D11Resource* d3d11Res = nullptr;
            if (colorTex->d3d11MSAATexture2D) {
                d3d11Res = colorTex->d3d11MSAATexture2D;
                D3D11_TEXTURE2D_DESC texDesc = { };
                colorTex->d3d11MSAATexture2D->GetDesc(&texDesc);
                rtvDesc.Format = texDesc.Format;
            }
            else if (colorTex->d3d11Texture2D) {
                d3d11Res = colorTex->d3d11Texture2D;
                D3D11_TEXTURE2D_DESC texDesc = { };
                colorTex->d3d11Texture2D->GetDesc(&texDesc);
                rtvDesc.Format = texDesc.Format;
            }
            else {
                d3d11Res = colorTex->d3d11Texture3D;
                D3D11_TEXTURE3D_DESC texDesc = { };
                colorTex->d3d11Texture3D->GetDesc(&texDesc);
                rtvDesc.Format = texDesc.Format;
            }
            o_assert_dbg(d3d11Res);
            switch (colorTex->Setup.Type) {
                case TextureType::Texture2D:
                    if (isMSAA) {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
                    }
                    else {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                        rtvDesc.Texture2D.MipSlice = rp.Setup.ColorAttachments[i].MipLevel;
                    }
                    break;
                case TextureType::TextureCube:
                    if (isMSAA) {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                        rtvDesc.Texture2DMSArray.FirstArraySlice = rp.Setup.ColorAttachments[i].Slice;
                        rtvDesc.Texture2DMSArray.ArraySize = 1;
                    }
                    else {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                        rtvDesc.Texture2DArray.MipSlice = rp.Setup.ColorAttachments[i].MipLevel;
                        rtvDesc.Texture2DArray.FirstArraySlice = rp.Setup.ColorAttachments[i].Slice;
                        rtvDesc.Texture2DArray.ArraySize = 1;
                    }
                    break;
                case TextureType::Texture3D:
                    o_assert_dbg(!isMSAA);
                    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
                    rtvDesc.Texture3D.MipSlice = rp.Setup.ColorAttachments[i].MipLevel;
                    rtvDesc.Texture3D.FirstWSlice = rp.Setup.ColorAttachments[i].Slice;
                    rtvDesc.Texture3D.WSize = 1;
                    break;
                case TextureType::TextureArray:
                    if (isMSAA) {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                        rtvDesc.Texture2DMSArray.FirstArraySlice = rp.Setup.ColorAttachments[i].Slice;
                        rtvDesc.Texture2DMSArray.ArraySize = 1;
                    }
                    else {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                        rtvDesc.Texture2DArray.MipSlice = rp.Setup.ColorAttachments[i].MipLevel;
                        rtvDesc.Texture2DArray.FirstArraySlice = rp.Setup.ColorAttachments[i].Slice;
                        rtvDesc.Texture2DArray.ArraySize = 1;
                    }
                default:
                    o_error("d3d11RenderPassFactory: invalid texture type\n");
                    break;
            }
            HRESULT hr = d3d11Device->CreateRenderTargetView(d3d11Res, &rtvDesc, &(rp.d3d11RenderTargetViews[i]));
            o_assert(SUCCEEDED(hr) && rp.d3d11RenderTargetViews[i]);
        } 
    }

    // create optional depth-stencil-view object
    if (rp.depthStencilTexture) {
        o_assert_dbg(nullptr == rp.d3d11DepthStencilView);
        texture* dsTex = rp.depthStencilTexture;
        o_assert_dbg(dsTex->d3d11DepthStencilTexture);
        D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc = { };
        D3D11_TEXTURE2D_DESC texDesc = { };
        dsTex->d3d11DepthStencilTexture->GetDesc(&texDesc);
        dsDesc.Format = texDesc.Format;
        dsDesc.Flags = 0;
        if (isMSAA) {
            dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            dsDesc.Texture2D.MipSlice = 0;
        }
        else {
            dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        } 
        HRESULT hr = d3d11Device->CreateDepthStencilView(dsTex->d3d11DepthStencilTexture, &dsDesc, &rp.d3d11DepthStencilView);
        o_assert(SUCCEEDED(hr) && rp.d3d11DepthStencilView); 
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11Factory::destroyRenderPass(renderPass& rp) {
    o_assert_dbg(this->isValid);
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        if (rp.d3d11RenderTargetViews[i]) {
            rp.d3d11RenderTargetViews[i]->Release();
        }
    }
    if (rp.d3d11DepthStencilView) {
        rp.d3d11DepthStencilView->Release();
    }
    gfxFactoryBase::destroyRenderPass(rp);
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11Factory::initShader(shader& shd) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nullptr == shd.d3d11VertexShader);
    o_assert_dbg(nullptr == shd.d3d11PixelShader);
    ID3D11Device* d3d11Device = this->pointers.renderer->d3d11Device;

    this->pointers.renderer->invalidateShaderState();
    const ShaderLang::Code slang = ShaderLang::HLSL5;
    const ShaderSetup& setup = shd.Setup;

    // create vertex shader
    const void* vsPtr = nullptr;
    uint32_t vsSize = 0;
    setup.VertexShaderByteCode(slang, vsPtr, vsSize);
    o_assert_dbg(vsPtr);
    HRESULT hr = d3d11Device->CreateVertexShader(vsPtr, vsSize, NULL, &shd.d3d11VertexShader);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(shd.d3d11VertexShader);

    // create pixel shader
    const void* psPtr = nullptr;
    uint32_t psSize = 0;
    setup.FragmentShaderByteCode(slang, psPtr, psSize);
    o_assert_dbg(psPtr);
    hr = d3d11Device->CreatePixelShader(psPtr, psSize, NULL, &shd.d3d11PixelShader);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(shd.d3d11PixelShader);

    // create constant buffers
    D3D11_BUFFER_DESC cbDesc = { };
    for (int i = 0; i < setup.NumUniformBlocks(); i++) {
        const ShaderStage::Code bindStage = setup.UniformBlockBindStage(i);
        const int bindSlot = setup.UniformBlockBindSlot(i);
        o_assert_dbg(InvalidIndex != bindSlot);
        const uint32 byteSize = setup.UniformBlockByteSize(i);
        o_assert_dbg(byteSize > 0);

        // NOTE: constant buffer size must be multiple of 16 bytes
        cbDesc.ByteWidth = Memory::RoundUp(byteSize, 16);
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = 0;

        ID3D11Buffer* d3d11ConstantBuffer = nullptr;
        hr = d3d11Device->CreateBuffer(&cbDesc, nullptr, &d3d11ConstantBuffer);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(d3d11ConstantBuffer);

        // the d3d11ConstantBuffer ptr can be 0 at this point, if the
        // uniform block only contains textures
        shd.addUniformBlockEntry(bindStage, bindSlot, d3d11ConstantBuffer);
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11Factory::destroyShader(shader& shd) {
    o_assert_dbg(this->isValid);
    this->pointers.renderer->invalidateShaderState();
    if (shd.d3d11VertexShader) {
        shd.d3d11VertexShader->Release();
    }
    if (shd.d3d11PixelShader) {
        shd.d3d11PixelShader->Release();
    }
    for (int bindStage = 0; bindStage < ShaderStage::NumShaderStages; bindStage++) {
        for (int bindSlot = 0; bindSlot < GfxConfig::MaxNumUniformBlocksPerStage; bindSlot++) {
            ID3D11Buffer* cb = shd.getConstantBuffer((ShaderStage::Code)bindStage, bindSlot);
            if (cb) {
                cb->Release();
            }
        }
    }
    shd.Clear();
}

//------------------------------------------------------------------------------
static ID3D11InputLayout*
createInputLayout(ID3D11Device* d3d11Device, const pipeline& pip) {
    o_assert_dbg(nullptr == pip.d3d11InputLayout);

    // create a new input layout object and add to cache
    D3D11_INPUT_ELEMENT_DESC d3d11Comps[VertexAttr::NumVertexAttrs] = { 0 };
    int d3d11CompIndex = 0;
    for (int mshSlotIndex = 0; mshSlotIndex < GfxConfig::MaxNumInputMeshes; mshSlotIndex++) {
        const VertexLayout& layout = pip.Setup.Layouts[mshSlotIndex];
        for (int compIndex = 0; compIndex < layout.NumComponents(); compIndex++, d3d11CompIndex++) {
            const auto& comp = layout.ComponentAt(compIndex);
            o_assert_dbg(d3d11CompIndex < VertexAttr::NumVertexAttrs);
            D3D11_INPUT_ELEMENT_DESC& d3d11Comp = d3d11Comps[d3d11CompIndex];
            d3d11Comp.SemanticName = d3d11Types::asSemanticName(comp.Attr);
            d3d11Comp.SemanticIndex = d3d11Types::asSemanticIndex(comp.Attr);
            d3d11Comp.Format = d3d11Types::asInputElementFormat(comp.Format);
            d3d11Comp.InputSlot = mshSlotIndex;
            d3d11Comp.AlignedByteOffset = layout.ComponentByteOffset(compIndex);
            d3d11Comp.InputSlotClass = d3d11Types::asInputClassification(layout.StepFunction);
            if (VertexStepFunction::PerVertex == layout.StepFunction) {
                d3d11Comp.InstanceDataStepRate = 0;
            }
            else {
                d3d11Comp.InstanceDataStepRate = layout.StepRate;
            }
        }
    }

    // lookup the vertex shader bytecode
    const void* vsByteCode = nullptr;
    uint32_t vsSize = 0;
    pip.shd->Setup.VertexShaderByteCode(ShaderLang::HLSL5, vsByteCode, vsSize);
    o_assert_dbg(vsByteCode && (vsSize > 0));

    // create d3d11 input layout object
    ID3D11InputLayout* d3d11InputLayout = nullptr;
    HRESULT hr = d3d11Device->CreateInputLayout(
        d3d11Comps,         // pInputElementDesc 
        d3d11CompIndex,     // NumElements
        vsByteCode,         // pShaderBytecodeWithInputSignature 
        vsSize,             // BytecodeLength
        &d3d11InputLayout);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != d3d11InputLayout);
    return d3d11InputLayout;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11Factory::initPipeline(pipeline& pip) {
    o_assert_dbg(nullptr == pip.d3d11InputLayout);
    o_assert_dbg(nullptr == pip.d3d11RasterizerState);
    o_assert_dbg(nullptr == pip.d3d11BlendState);
    o_assert_dbg(nullptr == pip.d3d11DepthStencilState);
    ID3D11Device* d3d11Device = this->pointers.renderer->d3d11Device;
    HRESULT hr;

    gfxFactoryBase::initPipeline(pip);
    o_assert_dbg(pip.shd);

    pip.d3d11InputLayout = createInputLayout(d3d11Device, pip);
    pip.d3d11PrimTopology = d3d11Types::asPrimitiveTopology(pip.Setup.PrimType);
    o_assert_dbg(pip.d3d11InputLayout);

    // create state objects (NOTE: creating the same state will return
    // the same d3d11 state object, so no need to implement our own reuse)
    const RasterizerState& rastState = pip.Setup.RasterizerState;
    D3D11_RASTERIZER_DESC rastDesc = { };
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = d3d11Types::asCullMode(rastState.CullFaceEnabled, rastState.CullFace);
    rastDesc.FrontCounterClockwise = FALSE;  // OpenGL convention
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.ScissorEnable = rastState.ScissorTestEnabled;
    rastDesc.MultisampleEnable = rastState.SampleCount > 1;
    rastDesc.AntialiasedLineEnable = FALSE;
    hr = d3d11Device->CreateRasterizerState(&rastDesc, &pip.d3d11RasterizerState);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != pip.d3d11RasterizerState);

    const DepthStencilState& dsState = pip.Setup.DepthStencilState;
    D3D11_DEPTH_STENCIL_DESC dsDesc = { };
    dsDesc.DepthEnable = TRUE;      // FIXME: have DepthTestEnable in RasterizerState?
    dsDesc.DepthWriteMask = dsState.DepthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = d3d11Types::asComparisonFunc(dsState.DepthCmpFunc);
    dsDesc.StencilEnable    = dsState.StencilEnabled;
    dsDesc.StencilReadMask  = dsState.StencilReadMask;
    dsDesc.StencilWriteMask = dsState.StencilWriteMask;
    dsDesc.FrontFace.StencilFailOp = d3d11Types::asStencilOp(dsState.StencilFront.FailOp);
    dsDesc.FrontFace.StencilDepthFailOp = d3d11Types::asStencilOp(dsState.StencilFront.DepthFailOp);
    dsDesc.FrontFace.StencilPassOp = d3d11Types::asStencilOp(dsState.StencilFront.PassOp);
    dsDesc.FrontFace.StencilFunc = d3d11Types::asComparisonFunc(dsState.StencilFront.CmpFunc);
    dsDesc.BackFace.StencilFailOp = d3d11Types::asStencilOp(dsState.StencilBack.FailOp);
    dsDesc.BackFace.StencilDepthFailOp = d3d11Types::asStencilOp(dsState.StencilBack.DepthFailOp);
    dsDesc.BackFace.StencilPassOp = d3d11Types::asStencilOp(dsState.StencilBack.PassOp);
    dsDesc.BackFace.StencilFunc = d3d11Types::asComparisonFunc(dsState.StencilBack.CmpFunc);
    hr = d3d11Device->CreateDepthStencilState(&dsDesc, &pip.d3d11DepthStencilState);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != pip.d3d11DepthStencilState);

    const BlendState& blendState = pip.Setup.BlendState;
    o_assert(blendState.MRTCount < GfxConfig::MaxNumColorAttachments);
    D3D11_BLEND_DESC blendDesc = { };
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = blendState.BlendEnabled;
    blendDesc.RenderTarget[0].SrcBlend = d3d11Types::asBlendFactor(blendState.SrcFactorRGB);
    blendDesc.RenderTarget[0].DestBlend = d3d11Types::asBlendFactor(blendState.DstFactorRGB);
    blendDesc.RenderTarget[0].BlendOp = d3d11Types::asBlendOp(blendState.OpRGB);
    blendDesc.RenderTarget[0].SrcBlendAlpha = d3d11Types::asBlendFactor(blendState.SrcFactorAlpha);
    blendDesc.RenderTarget[0].DestBlendAlpha = d3d11Types::asBlendFactor(blendState.DstFactorAlpha);
    blendDesc.RenderTarget[0].BlendOpAlpha = d3d11Types::asBlendOp(blendState.OpAlpha);
    blendDesc.RenderTarget[0].RenderTargetWriteMask = d3d11Types::asColorWriteMask(blendState.ColorWriteMask);
    hr = d3d11Device->CreateBlendState(&blendDesc, &pip.d3d11BlendState);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != pip.d3d11BlendState);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11Factory::destroyPipeline(pipeline& pip) {
    this->pointers.renderer->invalidatePipeline();
    if (nullptr != pip.d3d11InputLayout) {
        pip.d3d11InputLayout->Release();
    }
    if (nullptr != pip.d3d11RasterizerState) {
        pip.d3d11RasterizerState->Release();
    }
    if (nullptr != pip.d3d11BlendState) {
        pip.d3d11BlendState->Release();
    }
    if (nullptr != pip.d3d11DepthStencilState) {
        pip.d3d11DepthStencilState->Release();
    }
    gfxFactoryBase::destroyPipeline(pip);
}

} // namespace _priv
} // namespace Oryol
