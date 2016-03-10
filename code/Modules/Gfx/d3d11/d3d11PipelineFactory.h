#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11PipelineFactory
    @ingroup _priv
    @brief D3D11 implementation of pipelineFactory
*/
#include "Gfx/Resource/pipelineFactoryBase.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class d3d11PipelineFactory : public pipelineFactoryBase {
public:
    /// destructor
    ~d3d11PipelineFactory();

    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// setup pipeline resource
    ResourceState::Code SetupResource(pipeline& pip);
    /// destroy the pipeline
    void DestroyResource(pipeline& pip);

private:
    /// create or return existing input layout object
    ID3D11InputLayout* createInputLayout(const pipeline& pip);

    ID3D11Device* d3d11Device = nullptr;
};

} // namespace _priv
} // namespace Oryol

