#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::drawStateFactory
    @brief factory class for drawState resources
*/
#include "Render/base/simpleFactory.h"
#include "Render/Core/drawState.h"

namespace Oryol {
namespace Render {

class meshPool;
class programBundlePool;

class drawStateFactory : public simpleFactory<drawState, ResourceType::DrawState> {
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

} // namespace Render
} // namespace Oryol