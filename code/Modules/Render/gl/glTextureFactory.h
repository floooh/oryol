#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glTextureFactory
    @ingroup _priv
    @brief private: GL implementation of textureFactory
*/
#include "Render/base/renderLoaderFactory.h"
#include "Render/Core/texture.h"

namespace Oryol {
namespace _priv {

class stateWrapper;
class texture;
class textureLoaderBase;
class displayMgr;
class texturePool;
    
class glTextureFactory : public renderLoaderFactory<texture, textureLoaderBase, ResourceType::Texture> {
public:
    /// constructor
    glTextureFactory();
    /// destructor
    ~glTextureFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(stateWrapper* stWrapper, displayMgr* displayMgr, texturePool* texPool);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource, continue calling until res state is not Pending
    void SetupResource(texture& tex);
    /// setup with input data, continue calling until res state is not Pending
    void SetupResource(texture& tex, const Ptr<Stream>& data);
    /// discard the resource
    void DestroyResource(texture& tex);
    
    /// generate a new GL texture and bind to texture unit 0 (called by texture loaders)
    GLuint glGenAndBindTexture(GLenum target);

private:
    /// create a render target
    void createRenderTarget(texture& tex);
    /// create texture from raw pixel data
    void createFromPixelData(texture& tex, const Ptr<Stream>& data);

    stateWrapper* glStateWrapper;
    displayMgr* displayManager;
    texturePool* texPool;
    bool isValid;
};
    
} // namespace _priv
} // namespace Oryol

