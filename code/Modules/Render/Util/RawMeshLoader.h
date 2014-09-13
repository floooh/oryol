#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::RawMeshLoader
    @ingroup Render
    @brief mesh loader for raw data created with MeshBuilder
*/
#include "Render/base/meshLoaderBase.h"
#include "Core/Creator.h"

namespace Oryol {
    
class RawMeshLoader : public _priv::meshLoaderBase {
    OryolClassDecl(RawMeshLoader);
    OryolClassCreator(RawMeshLoader);
public:
    /// test if the loader accepts the resource
    virtual bool Accepts(const _priv::mesh& mesh) const override;
    /// test if the loader accepts the resource (with stream data)
    virtual bool Accepts(const _priv::mesh& mesh, const Ptr<Stream>& data) const override;
    /// setup the mesh object (currently not supported)
    virtual void Load(_priv::mesh& mesh) const override;
    /// setup the mesh object from data in stream
    virtual void Load(_priv::mesh& mesh, const Ptr<Stream>& data) const override;
};
    
} // namespace Oryol
 
 