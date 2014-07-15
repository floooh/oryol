#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::meshLoaderBase
    @brief private: base class for mesh loaders
*/
#include "Core/RefCounted.h"
#include "Render/Core/mesh.h"
#include "IO/Stream/Stream.h"

namespace Oryol {
namespace Render {

class meshFactory;

class meshLoaderBase : public Core::RefCounted {
public:
    /// constructor
    meshLoaderBase();
    /// destructor
    virtual ~meshLoaderBase();
    
    /// test if the loader accepts the resource
    virtual bool Accepts(const mesh& resource) const = 0;
    /// test if the loader accepts the resource, with data stream
    virtual bool Accepts(const mesh& resource, const Core::Ptr<IO::Stream>& data) const = 0;
    /// start to load, or continue loading
    virtual void Load(mesh& resource) const = 0;
    /// start to load, or continue loading, with data stream
    virtual void Load(mesh& resource, const Core::Ptr<IO::Stream>& data) const = 0;

    /// called when attached to factory
    void onAttachToFactory(meshFactory* factory);
    /// called when detached from factory
    void onDetachFromFactory();

protected:
    meshFactory* mshFactory;
};
    
} // namespace Render
} // namespace Oryol
