#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::blendStateFactory
    @brief factory class for blendState resource
*/
#include "Resource/simpleFactory.h"
#include "Render/Core/blendState.h"

namespace Oryol {
namespace Render {

class blendStateFactory : public Resource::simpleFactory<blendState> {
public:
    /// constructor
    blendStateFactory();
    /// destructor
    ~blendStateFactory();
    
    /// get the resource type this factory produces
    uint16 GetResourceType() const;
    
    /// setup the factory
    void Setup();
    /// discard the factory
    void Discard();
    /// return true if factory is valid
    bool IsValid() const;
    
    /// setup shader resource
    void SetupResource(blendState& bs);
    /// destroy the shader
    void DestroyResource(blendState& bs);
    
private:
    bool isValid;
};

} // namespace Render
} // namespace Oryol
