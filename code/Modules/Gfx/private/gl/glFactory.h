#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glFactory.h
    @ingroup _priv
    @brief GL implementation of gfxFactory
*/
#include "Gfx/private/gfxFactoryBase.h"
#include "Gfx/private/gl/gl_decl.h"

namespace Oryol {
namespace _priv {

class mesh;
class texture;
class renderPass;
class shader;
class pipeline;

class glFactory : public gfxFactoryBase {
public:
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
    GLuint createBuffer(GLenum type, const void* data, uint32_t dataSize, Usage::Code usage);
};

}
} // namespace Oryol
