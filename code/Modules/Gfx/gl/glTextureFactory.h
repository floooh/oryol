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
    ResourceState::Code SetupResource(texture& tex, const void* data, int32_t size);
    /// discard the resource
    void DestroyResource(texture& tex);
    
    /// generate a new GL texture and bind to texture unit 0 (called by texture loaders)
    GLuint glGenAndBindTexture(GLenum target);

private:
    /// helper method to setup texture params on GL texture
    void setupTextureParams(const TextureSetup& setup, GLenum glTexTarget, GLuint glTex);
    /// helper method to setup texture params on GL texture
    void setupTextureAttrs(texture& tex);
    /// create a texture with or without associated data
    ResourceState::Code createTexture(texture& tex, const void* data, int32_t size);

    gfxPointers pointers;
    bool isValid = false;
};
    
} // namespace _priv
} // namespace Oryol

