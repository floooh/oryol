#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawStateFactoryBase
    @ingroup _priv
    @brief base class for drawStateFactory
*/
#include "Resource/ResourceState.h"

namespace Oryol {
namespace _priv {

class renderer;
class meshPool;
class programBundlePool;
class drawState;

class drawStateFactoryBase {
public:
    /// constructor
    drawStateFactoryBase();
    /// destructor
    ~drawStateFactoryBase();

    /// setup the factory
    void Setup(renderer* rendr, meshPool* mshPool, programBundlePool* pbPool);
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

    class renderer* renderer;
    class meshPool* meshPool;
    class programBundlePool* programBundlePool;
    bool isValid;
};

} // namespace _priv
} // namespace Oryol