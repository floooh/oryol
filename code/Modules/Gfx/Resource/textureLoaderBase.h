#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureLoaderBase
    @ingroup _priv
    @brief base class for texture loaders
*/
#include "Gfx/Resource/loaderBase.h"
#include "Gfx/Resource/texture.h"
#include "IO/Stream/Stream.h"

namespace Oryol {
namespace _priv {

class texture;
class textureFactory;

class textureLoaderBase : public loaderBase {
public:
    /// constructor
    textureLoaderBase();
    /// destructor
    virtual ~textureLoaderBase();
    
    /// resource type of this loader
    ResourceType::Code resourceType() const {
        return ResourceType::Texture;
    };
    
    /// test if the loader accepts the resource
    virtual bool Accepts(const texture& resource) const = 0;
    /// test if the loader accepts the resource, with data stream
    virtual bool Accepts(const texture& resource, const Ptr<Stream>& data) const = 0;
    /// start to load, or continue loading
    virtual void Load(texture& resource) const = 0;
    /// start to load, or continue loading, with data stream
    virtual void Load(texture& resource, const Ptr<Stream>& data) const = 0;
    
    /// called when attached to factory
    void onAttachToFactory(textureFactory* factory);
    /// called when detached from factory
    void onDetachFromFactory();
    
protected:
    textureFactory* texFactory;
};

} // namespace _priv
} // namespace Oryol
