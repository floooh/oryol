#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::MeshLoaderBase
    @ingroup Gfx
    @brief base class for Gfx mesh loaders
*/
#include "Resource/ResourceLoader.h"
#include "Gfx/GfxTypes.h"
#include <functional>

namespace Oryol {

class MeshLoaderBase : public ResourceLoader {
    OryolClassDecl(MeshLoaderBase);
public:
    /// optional callback when loading has succeeded
    typedef std::function<void(MeshSetup&)> LoadedFunc;

    /// constructor
    MeshLoaderBase(const MeshSetup& setup);
    /// constructor with success callback
    MeshLoaderBase(const MeshSetup& setup, LoadedFunc onLoaded);
    /// return resource locator
    virtual class Locator Locator() const override;

protected:
    MeshSetup setup;
    std::function<void(MeshSetup&)> onLoaded;
};

} // namespace Oryol