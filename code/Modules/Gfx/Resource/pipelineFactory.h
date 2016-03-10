#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pipelineFactory
    @ingroup _priv
    @brief factory class for pipeline resource objects
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glPipelineFactory.h"
namespace Oryol {
namespace _priv {
class pipelineFactory : public glPipelineFactory { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11PipelineFactory.h"
namespace Oryol {
namespace _priv {
class pipelineFactory : public d3d11PipelineFactory { };
} }
#elif ORYOL_D3D12
#include "Gfx/d3d12/d3d12PipelineFactory.h"
namespace Oryol {
namespace _priv {
class pipelineFactory : public d3d12PipelineFactory { };
} }
#elif ORYOL_METAL
#include "Gfx/mtl/mtlPipelineFactory.h"
namespace Oryol {
namespace _priv {
class pipelineFactory : public mtlPipelineFactory { };
} }
#else
#error "Target platform not yet supported!"
#endif
