#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::textureLoaderBase
    @brief private: base class for texture loaders
*/
#include "Core/RefCounted.h"
#include "Render/Core/mesh.h"
#include "IO/Stream/Stream.h"

namespace Oryol {
namespace Render {

class texture;
class textureFactory;

class textureLoaderBase : public Core::RefCounted {
public:
    /// constructor
    textureLoaderBase();
    /// destructor
    virtual ~textureLoaderBase();
    
    /// test if the loader accepts the resource
    virtual bool Accepts(const texture& resource) const = 0;
    /// test if the loader accepts the resource, with data stream
    virtual bool Accepts(const texture& resource, const Core::Ptr<IO::Stream>& data) const = 0;
    /// start to load, or continue loading
    virtual void Load(texture& resource) const = 0;
    /// start to load, or continue loading, with data stream
    virtual void Load(texture& resource, const Core::Ptr<IO::Stream>& data) const = 0;
    
    /// called when attached to factory
    void onAttachToFactory(textureFactory* factory);
    /// called when detached from factory
    void onDetachFromFactory();
    
protected:
    textureFactory* texFactory;
};

} // namespace Render
} // namespace Oryol
