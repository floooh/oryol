#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::stateBlockFactory
    @brief private: resource factory for StateBlock objects
*/
#include "Resource/simpleFactory.h"
#include "Render/Core/stateBlock.h"

namespace Oryol {
namespace Render {
    
class stateBlockFactory : public Resource::simpleFactory<stateBlock> {
public:
    /// constructor
    stateBlockFactory();
    /// destructor
    ~stateBlockFactory();
    
    /// get the resource type this factory produces
    uint16 GetResourceType() const;
    
    /// setup the factory
    void Setup();
    /// discard the factory
    void Discard();
    /// return true if factory is valid
    bool IsValid() const;
    
    /// setup shader resource (directly setup shader, no loaders)
    void SetupResource(stateBlock& sb);
    /// destroy the shader
    void DestroyResource(stateBlock& sb);

private:
    bool isValid;
};
    
} // namespace Render
} // namespace Oryol