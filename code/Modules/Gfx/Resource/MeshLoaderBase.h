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
    /// constructor
    MeshLoaderBase(const MeshSetup& setup, int32 ioLane);
    /// return resource locator
    virtual const class Locator& Locator() const;

protected:
    MeshSetup setup;
    int32 ioLane;
};

} // namespace Oryol