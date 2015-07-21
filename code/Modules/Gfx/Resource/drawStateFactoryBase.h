#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawStateFactoryBase
    @ingroup _priv
    @brief base class for drawStateFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class drawState;

class drawStateFactoryBase {
public:
    /// constructor
    drawStateFactoryBase();
    /// destructor
    ~drawStateFactoryBase();

    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if factory has been setup
    bool IsValid() const;
    /// setup shader resource
    ResourceState::Code SetupResource(drawState& ds);
    /// destroy the shader
    void DestroyResource(drawState& ds);
    
protected:
    /// resolve input mesh pointers
    void resolveInputMeshes(drawState& ds);
    /// check whether input mesh configuration is valid
    void checkInputMeshes(const drawState& ds) const;

    gfxPointers pointers;
    bool isValid;
};

} // namespace _priv
} // namespace Oryol