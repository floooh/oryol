#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glTextureFactory
    @ingroup _priv
    @brief private: GL implementation of textureFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Setup/TextureSetup.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/gl/gl_decl.h"

namespace Oryol {
namespace _priv {

class texture;
    
class glTextureFactory {
public:
    /// constructor
    glTextureFactory();
    /// destructor
    ~glTextureFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource
    ResourceState::Code SetupResource(texture& tex);
    /// setup with input data
    ResourceState::Code SetupResource(texture& tex, const void* data, int32 size);
    /// discard the resource
    void DestroyResource(texture& tex);
    
    /// generate a new GL texture and bind to texture unit 0 (called by texture loaders)
    GLuint glGenAndBindTexture(GLenum target);

private:
    /// helper method to setup texture params on GL texture
    void setupTextureParams(const TextureSetup& setup, GLuint glTex);
    /// helper method to setup texture params on GL texture
    void setupTextureAttrs(texture& tex);
    /// create a render target
    ResourceState::Code createRenderTarget(texture& tex);
    /// create texture from raw pixel data
    ResourceState::Code createFromPixelData(texture& tex, const void* data, int32 size);
    /// create an empty texture (cannot be an immutable texture)
    ResourceState::Code createEmptyTexture(texture& tex);

    gfxPointers pointers;
    bool isValid;
};
    
} // namespace _priv
} // namespace Oryol

