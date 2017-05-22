#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlFactory
    @ingroup _priv
    @brief Metal implementation of gfxFactory
*/
#include "Gfx/private/gfxFactoryBase.h"
#include "Gfx/private/mtl/mtl_decl.h"
#include "Core/Containers/Queue.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace _priv {

class mesh;
class texture;
class renderPass;
class shader;
class pipeline;

class mtlFactory : public gfxFactoryBase {
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
    /// initialize a new shader object
    ResourceState::Code initShader(shader& shd);
    /// destroy a shader object
    void destroyShader(shader& shd);
    /// initialize a new pipeline object
    ResourceState::Code initPipeline(pipeline& pip);
    /// destroy a pipeline object
    void destroyPipeline(pipeline& pip);

    /// add an id to be released when no longer used by GPU
    void releaseDeferred(ORYOL_OBJC_ID obj);
    /// garbage collect deferred-released Metal resources
    void garbageCollect();

    /// helper method to setup a mesh object as fullscreen quad
    ResourceState::Code initFullscreenQuad(mesh& mesh);
    /// helper method to create a standard mesh
    ResourceState::Code initStdMesh(mesh& mesh, const void* data, int size);
    /// helper method to create a vertex buffer object
    ORYOL_OBJC_TYPED_ID(MTLBuffer) createBuffer(const void* data, uint32_t dataSize, Usage::Code usage);

    /// helper method to create a sampler state object and set in texture object
    void createSamplerState(texture& tex);
    /// helper method to release sampler state object of texture
    void releaseSamplerState(texture& tex);

    struct freeItem {
        freeItem();
        freeItem(int frameIndex, ORYOL_OBJC_ID obj);
        ~freeItem();

        int frameIndex;
        ORYOL_OBJC_ID obj;
    };
    Queue<freeItem> releaseQueue;
    
    struct SamplerCacheItem {
        ORYOL_OBJC_TYPED_ID(MTLSamplerState) mtlSamplerState;
        int useCount;
    };
    Map<uint32_t, SamplerCacheItem> samplerCache;
};

} // namespace _priv
} // namespace Oryol

