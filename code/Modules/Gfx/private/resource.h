#pragma once
//------------------------------------------------------------------------------
#if ORYOL_OPENGL
#include "Gfx/private/gl/glResource.h"
#elif ORYOL_D3D11
#include "Gfx/private/d3d11/d3d11Resource.h"
#elif ORYOL_METAL
#include "Gfx/private/mtl/mtlResource.h"
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
#elif ORYOL_METAL
class mesh : public mtlMesh { };
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
#elif ORYOL_METAL
class pipeline : public mtlPipeline { };
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
#elif ORYOL_METAL
class shader : public mtlShader { };
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
#elif ORYOL_METAL
class texture : public mtlTexture { };
#else
#error "Target platform not yet supported!"
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::renderPass
    @ingroup _priv
    @brief render-pass frontend class
*/
#if ORYOL_OPENGL
class renderPass : public glRenderPass { };
#elif ORYOL_D3D11
class renderPass : public d3d11RenderPass { };
#elif ORYOL_METAL
class renderPass : public mtlRenderPass { };
#else
class renderPass : public renderPassBase { };
#endif

} // namespace _priv
} // namespace Oryol

