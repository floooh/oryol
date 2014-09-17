#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::meshLoaderBase
    @ingroup _priv
    @brief private: base class for mesh loaders
*/
#include "Gfx/base/loaderBase.h"
#include "Gfx/Core/mesh.h"
#include "IO/Stream/Stream.h"

namespace Oryol {
namespace _priv {

class meshFactory;

class meshLoaderBase : public loaderBase {
public:
    /// constructor
    meshLoaderBase();
    /// destructor
    virtual ~meshLoaderBase();
    
    /// resource type of this loader
    ResourceType::Code resourceType() const {
        return ResourceType::Mesh;
    };
    
    /// test if the loader accepts the resource
    virtual bool Accepts(const mesh& resource) const = 0;
    /// test if the loader accepts the resource, with data stream
    virtual bool Accepts(const mesh& resource, const Ptr<Stream>& data) const = 0;
    /// start to load, or continue loading
    virtual void Load(mesh& resource) const = 0;
    /// start to load, or continue loading, with data stream
    virtual void Load(mesh& resource, const Ptr<Stream>& data) const = 0;

    /// called when attached to factory
    void onAttachToFactory(meshFactory* factory);
    /// called when detached from factory
    void onDetachFromFactory();

protected:
    meshFactory* mshFactory;
};
    
} // namespace _priv
} // namespace Oryol
