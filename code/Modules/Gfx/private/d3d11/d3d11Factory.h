#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Factory
    @ingroup _priv
    @brief D3D11 implementation of gfxFactory
*/
#include "Gfx/private/gfxFactoryBase.h"
#include "Gfx/private/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class mesh;
class texture;
class renderPass;
class shader;
class pipeline;

class d3d11Factory : public gfxFactoryBase {
public:
    /// setup the factory
    void setup(const gfxPointers& ptrs);
    /// discard the factory
    void discard();

    /// initialize new mesh object
    ResourceState::Code initMesh(mesh& msh, const void* data, int size);
    /// destroy a mesh object
    void destroyMesh(mesh& msh);
    /// initialize a new texture object
    ResourceState::Code initTexture(texture& tex, const void* data, int size);
    /// destroy a texture object
    void destroyTexture(texture& tex);
    /// initialize a new render pass object
    ResourceState::Code initRenderPass(renderPass& rp);
    /// destroy a render pass object
    void destroyRenderPass(renderPass& rp);
    /// initialize a new shader object
    ResourceState::Code initShader(shader& shd);
    /// destroy a shader object
    void destroyShader(shader& shd);
    /// initialize a new pipeline object
    ResourceState::Code initPipeline(pipeline& pip);
    /// destroy a pipeline object
    void destroyPipeline(pipeline& pip);

    /// helper method to setup a mesh object as fullscreen quad
    ResourceState::Code initFullscreenQuad(mesh& mesh);
    /// helper method to create a standard mesh
    ResourceState::Code initStdMesh(mesh& mesh, const void* data, int size);
    /// helper method to create vertex or index buffer
    ID3D11Buffer* createBuffer(const void* vertexData, uint32_t vertexDataSize, uint32_t d3d11BindFlags, Usage::Code usage);

    static const int maxNumSubResourceData = GfxConfig::MaxNumTextureArraySlices * GfxConfig::MaxNumTextureMipMaps;
    D3D11_SUBRESOURCE_DATA* subResourceData = nullptr;
};

} // namespace _priv
} // namespace Oryol
