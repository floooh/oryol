#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::RawMeshLoader
    @brief mesh loader for raw data created with MeshBuilder
*/
#include "Render/base/meshLoaderBase.h"
#include "Core/Creator.h"

namespace Oryol {
namespace Render {
    
class meshFactory;
    
class RawMeshLoader : public meshLoaderBase {
    OryolClassDecl(RawMeshLoader);
    OryolClassCreator(RawMeshLoader);
public:
    /// test if the loader accepts the resource
    virtual bool Accepts(const mesh& mesh) const override;
    /// test if the loader accepts the resource (with stream data)
    virtual bool Accepts(const mesh& mesh, const Core::Ptr<IO::Stream>& data) const override;
    /// setup the mesh object (currently not supported)
    virtual void Load(mesh& mesh) const override;
    /// setup the mesh object from data in stream
    virtual void Load(mesh& mesh, const Core::Ptr<IO::Stream>& data) const override;
};
    
} // namespace Render
} // namespace Oryol
 
 