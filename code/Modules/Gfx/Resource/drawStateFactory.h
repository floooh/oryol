#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawStateFactory
    @ingroup _priv
    @brief factory class for drawState resources
*/
#include "Resource/ResourceState.h"
#include "Gfx/Resource/drawState.h"

namespace Oryol {
namespace _priv {

class meshPool;
class programBundlePool;

class drawStateFactory {
public:
    /// constructor
    drawStateFactory();
    /// destructor
    ~drawStateFactory();
    /// setup the factory
    void Setup(meshPool* mshPool, programBundlePool* pbPool);
    /// discard the factory
    void Discard();
    /// setup shader resource
    ResourceState::Code SetupResource(drawState& ds);
    /// destroy the shader
    void DestroyResource(drawState& ds);
    
private:
    class meshPool* meshPool;
    class programBundlePool* programBundlePool;
};

} // namespace _priv
} // namespace Oryol