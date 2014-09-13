#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glTextureLoader
    @ingroup _priv
    @brief standard texture file loader for GL

    This loads the most common runtime texture file formats for the
    target platform (such as DDS, PVR, ...) into GL textures.
*/
#include "Render/base/textureLoaderBase.h"

namespace gliml {
class context;
}

namespace Oryol {
namespace _priv {
    
class glTextureLoader : public textureLoaderBase {
    OryolClassDecl(glTextureLoader);
public:
    /// test if the loader accepts the resource
    virtual bool Accepts(const texture& tex) const override;
    /// test if the loader accepts the resource (with stream data)
    virtual bool Accepts(const texture& tex, const Ptr<Stream>& data) const override;
    /// setup the texture object
    virtual void Load(texture& tex) const override;
    /// setup the mesh object from data in stream
    virtual void Load(texture& tex, const Ptr<Stream>& data) const override;
private:
    /// create GL texture from gliml::context
    bool glCreateTexture(texture& tex, const gliml::context& ctx) const;
};

} // namespace _priv
} // namespace Oryol
