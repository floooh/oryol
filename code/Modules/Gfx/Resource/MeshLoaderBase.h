#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::MeshLoaderBase
    @ingroup Gfx
    @brief base class for Gfx mesh loaders
*/
#include "Resource/Core/ResourceLoader.h"
#include "Gfx/Setup/MeshSetup.h"

namespace Oryol {

class MeshLoaderBase : public ResourceLoader {
    OryolClassDecl(MeshLoaderBase);
public:
    /// prepare the loader
    virtual void Prepare(const Id& id, const MeshSetup& setup);
    
protected:
    MeshSetup setup;
    Id id;
};

} // namespace Oryol