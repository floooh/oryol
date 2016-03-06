#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pipeline
    @ingroup _priv
    @brief wraps all the pipeline state required for rendering
*/
#if ORYOL_OPENGL
#include "Gfx/gl/glPipeline.h"
namespace Oryol {
namespace _priv {
class pipeline : public glPipeline { };
} }
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11Pipeline.h"
namespace Oryol {
namespace _priv {
class pipeline : public d3d11Pipeline { };
} }
#elif ORYOL_D3D12
#include "Gfx/d3d12/d3d12Pipeline.h"
namespace Oryol {
namespace _priv {
class pipeline : public d3d12Pipeline { };
} }
#elif ORYOL_METAL
#include "Gfx/mtl/mtlPipeline.h"
namespace Oryol {
namespace _priv {
class pipeline : public mtlPipeline { };
} }
#else
#error "Target platform not yet supported!"
#endif
