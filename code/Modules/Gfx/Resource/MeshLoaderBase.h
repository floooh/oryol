#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::MeshLoaderBase
    @ingroup Gfx
    @brief base class for Gfx mesh loaders
*/
#include "Resource/Core/ResourceLoader.h"
#include "Resource/Core/SetupAndStream.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Setup/MeshSetup.h"

namespace Oryol {

class MeshLoaderBase : public ResourceLoader {
    OryolClassDecl(MeshLoaderBase);
public:
    /// indicate supported resource types
    bool Supports(GfxResourceType::Code resType) const;
    /// test if a resource loading request would be accepted
    virtual bool Accepts(const SetupAndStream<MeshSetup>& input) const;
    /// perform loading
    virtual SetupAndStream<MeshSetup> Load(const SetupAndStream<MeshSetup>& input) const;
};

} // namespace Oryol