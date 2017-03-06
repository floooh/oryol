#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11TextureFactory
    @ingroup _priv
    @brief D3D11 implementation of textureFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class texture;
    
class d3d11TextureFactory {
public:
    /// destructor
    ~d3d11TextureFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource
    ResourceState::Code SetupResource(texture& tex);
    /// setup with input data
    ResourceState::Code SetupResource(texture& tex, const void* data, int size);
    /// discard the resource
    void DestroyResource(texture& tex);

private:
    /// setup TextureAttrs of tex
    void setupTextureAttrs(texture& tex);
    /// create a texture with or without associated data
    ResourceState::Code createTexture(texture& tex, const void* data, int32_t size);
    /// create d3d11 sampler state object in texture
    ID3D11SamplerState* createSamplerState(const texture& tex);
    /// create d3d11 shader-resource-view object
    ID3D11ShaderResourceView* createShaderResourceView(const texture& tex, ID3D11Resource* d3d11Tex, DXGI_FORMAT d3d11Format);
    /// create d3d11 render-target-view object
    ID3D11RenderTargetView* createRenderTargetView(const texture& tex, ID3D11Resource* d3d11Tex, DXGI_FORMAT d3d11Format);
    /// create d3d11 depth-stencil-view object
    ID3D11DepthStencilView* createDepthStencilView(const texture& tex, ID3D11Texture2D* d3d11Tex, const D3D11_TEXTURE2D_DESC* texDesc);

    gfxPointers pointers;
    ID3D11Device* d3d11Device = nullptr;
    bool isValid = false;
};
    
} // namespace _priv
} // namespace Oryol
