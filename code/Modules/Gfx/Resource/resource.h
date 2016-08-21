#pragma once
//------------------------------------------------------------------------------
#if ORYOL_OPENGL
#include "Gfx/gl/glResource.h"
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11Resource.h"
#elif ORYOL_D3D12
#include "Gfx/d3d12/d3d12Resource.h"
#elif ORYOL_METAL
#include "Gfx/mtl/mtlResource.h"
#elif ORYOL_VULKAN
#include "Gfx/vlk/vlkResource.h"
#else
#error "Target platform not yet supported!"
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::Mesh
    @ingroup _priv
    @brief geometry mesh for rendering
    
    A Mesh object holds one or more vertex buffers, an optional 
    index buffer, and one or more primitive groups.
    @todo: describe mesh creation etc...
*/
namespace Oryol {
namespace _priv {

#if ORYOL_OPENGL
class mesh : public glMesh { };
#elif ORYOL_D3D11
class mesh : public d3d11Mesh { };
#elif ORYOL_D3D12
class mesh : public d3d12Mesh { };
#elif ORYOL_METAL
class mesh : public mtlMesh { };
#elif ORYOL_VULKAN
class mesh : public vlkMesh { };
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pipeline
    @ingroup _priv
    @brief wraps all the pipeline state required for rendering
*/
#if ORYOL_OPENGL
class pipeline : public glPipeline { };
#elif ORYOL_D3D11
class pipeline : public d3d11Pipeline { };
#elif ORYOL_D3D12
class pipeline : public d3d12Pipeline { };
#elif ORYOL_METAL
class pipeline : public mtlPipeline { };
#elif ORYOL_VULKAN
class pipeline : public vlkPipeline { };
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shader
    @brief Oryol shader resource class

    A shader resource holds several related shader programs, one of
    which is selected by a bit mask. Selection mask bits are usually
    associated with a specific shader feature, e.g. "skinning", 
    "normal-depth rendering", "shadow-map rendering", etc... The program
    bundle also maps shader variables to common slot indices across
    all contained programs.
*/
#if ORYOL_OPENGL
class shader : public glShader { };
#elif ORYOL_D3D11
class shader : public d3d11Shader { };
#elif ORYOL_D3D12
class shader : public d3d12Shader { };
#elif ORYOL_METAL
class shader : public mtlShader { };
#elif ORYOL_VULKAN
class shader : public vlkShader { };
#else
#error "Target platform not yet supported!"
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::texture
    @ingroup _priv
    @brief texture frontend class
    
    A texture object can be a normal 2D, 3D or cube texture, as well
    as a render target with optional depth buffer.
*/
#if ORYOL_OPENGL
class texture : public glTexture { };
#elif ORYOL_D3D11
class texture : public d3d11Texture { };
#elif ORYOL_D3D12
class texture : public d3d12Texture { };
#elif ORYOL_METAL
class texture : public mtlTexture { };
#elif ORYOL_VULKAN
class texture : public vlkTexture { };
#endif

} // namespace _priv
} // namespace Oryol

