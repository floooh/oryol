#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Texture
    @ingroup _priv
    @brief D3D11 implementation of texture
*/
#include "Gfx/Resource/textureBase.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class d3d11Texture : public textureBase {
public:
    /// constructor
    d3d11Texture();
    /// destructor
    ~d3d11Texture();

    /// clear the object
    void Clear();

    /// d3d11 2D texture object
    ID3D11Texture2D* d3d11Texture2D;
    /// d3d11 shader resource view object
    ID3D11ShaderResourceView* d3d11ShaderResourceView;
    /// d3d11 sampler state object
    ID3D11SamplerState* d3d11SamplerState;

    /// d3d11 render target resource view (only if this is a render-target texture)
    ID3D11RenderTargetView* d3d11RenderTargetView;
    /// d3d11 depth buffer texture (if render target with depth buffer)
    ID3D11Texture2D* d3d11DepthBufferTexture;
    /// d3d11 depth-stencil view
    ID3D11DepthStencilView* d3d11DepthStencilView;
};

} // namespace _priv
} // namespace Oryol