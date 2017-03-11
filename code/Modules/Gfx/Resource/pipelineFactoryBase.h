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
    /// constructor
    pipelineFactoryBase();
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
    bool isValid;
};

} // namespace _priv
} // namespace Oryol
