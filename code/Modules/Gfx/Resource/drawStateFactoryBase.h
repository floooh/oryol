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
    /// setup shader resource
    ResourceState::Code SetupResource(drawState& ds);
    /// destroy the shader
    void DestroyResource(drawState& ds);
    
protected:
    class renderer* renderer;
    class meshPool* meshPool;
    class programBundlePool* programBundlePool;
};

} // namespace _priv
} // namespace Oryol