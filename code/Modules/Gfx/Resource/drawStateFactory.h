#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawStateFactory
    @ingroup _priv
    @brief factory class for drawState resources
*/
#include "Gfx/Resource/renderSimpleFactory.h"
#include "Gfx/Resource/drawState.h"

namespace Oryol {
namespace _priv {

class meshPool;
class programBundlePool;

class drawStateFactory : public renderSimpleFactory<drawState, ResourceType::DrawState> {
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
    void SetupResource(drawState& ds);
    /// destroy the shader
    void DestroyResource(drawState& ds);
    
private:
    class meshPool* meshPool;
    class programBundlePool* programBundlePool;
};

} // namespace _priv
} // namespace Oryol