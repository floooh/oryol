#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::MeshLoaderBase
    @ingroup Gfx
    @brief base class for Gfx mesh loaders
*/
#include "Resource/Core/ResourceLoader.h"
#include "Gfx/Setup/MeshSetup.h"
#include <functional>

namespace Oryol {

class MeshLoaderBase : public ResourceLoader {
    OryolClassDecl(MeshLoaderBase);
public:
    /// optional callback when loading has succeeded
    typedef std::function<void(MeshSetup&)> LoadedFunc;

    /// constructor
    MeshLoaderBase(const MeshSetup& setup, int32 ioLane);
    /// constructor with success callback
    MeshLoaderBase(const MeshSetup& setup, int32 ioLane, LoadedFunc onLoaded);
    /// return resource locator
    virtual class Locator Locator() const override;

protected:
    MeshSetup setup;
    int32 ioLane;
    std::function<void(MeshSetup&)> onLoaded;
};

} // namespace Oryol