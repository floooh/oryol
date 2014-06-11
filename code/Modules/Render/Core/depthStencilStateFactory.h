#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::depthStencilStateFactory
    @brief private: resource factory for depthStencilState objects
*/
#include "Resource/simpleFactory.h"
#include "Render/Core/depthStencilState.h"

namespace Oryol {
namespace Render {
    
class depthStencilStateFactory : public Resource::simpleFactory<depthStencilState> {
public:
    /// constructor
    depthStencilStateFactory();
    /// destructor
    ~depthStencilStateFactory();
    
    /// get the resource type this factory produces
    uint16 GetResourceType() const;
    
    /// setup the factory
    void Setup();
    /// discard the factory
    void Discard();
    /// return true if factory is valid
    bool IsValid() const;
    
    /// setup shader resource (directly setup shader, no loaders)
    void SetupResource(depthStencilState& dds);
    /// destroy the shader
    void DestroyResource(depthStencilState& dds);
    
private:
    bool isValid;
};
    
} // namespace Render
} // namespace Oryol