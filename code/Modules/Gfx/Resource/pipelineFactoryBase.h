#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pipelineFactoryBase
    @ingroup _priv
    @brief base class for pipelineFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class pipeline;

class pipelineFactoryBase {
public:
    /// destructor
    ~pipelineFactoryBase();

    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// setup shader resource
    ResourceState::Code SetupResource(pipeline& pip);
    /// destroy the shader
    void DestroyResource(pipeline& pip);
    
protected:
    gfxPointers pointers;
    bool isValid = false;
};

} // namespace _priv
} // namespace Oryol
