#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Shader
    @ingroup _priv
    @brief D3D11 implementation of class shader
*/
#include "Gfx/Resource/shaderBase.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class d3d11Shader : public shaderBase {
public:
    /// constructor
    d3d11Shader();
    /// destructor
    ~d3d11Shader();

    /// clear the object
    void Clear();
    /// pointer to vertex shader (nullptr if this is a pixel shader)
    ID3D11VertexShader* d3d11VertexShader;
    /// pointer to pixel shader (nullptr if this is a vertex shader)
    ID3D11PixelShader* d3d11PixelShader;
};

} // namespace _priv
} // namespace Oryol