#pragma once
//------------------------------------------------------------------------------
#if ORYOL_OPENGL
#include "Gfx/gl/glMeshFactory.h"
#include "Gfx/gl/glPipelineFactory.h"
#include "Gfx/gl/glShaderFactory.h"
#include "Gfx/gl/glTextureFactory.h"
#elif ORYOL_D3D11
#include "Gfx/d3d11/d3d11MeshFactory.h"
#include "Gfx/d3d11/d3d11PipelineFactory.h"
#include "Gfx/d3d11/d3d11ShaderFactory.h"
#include "Gfx/d3d11/d3d11TextureFactory.h"
#elif ORYOL_D3D12
#include "Gfx/d3d12/d3d12MeshFactory.h"
#include "Gfx/d3d12/d3d12PipelineFactory.h"
#include "Gfx/d3d12/d3d12ShaderFactory.h"
#include "Gfx/d3d12/d3d12TextureFactory.h"
#elif ORYOL_METAL
#include "Gfx/mtl/mtlMeshFactory.h"
#include "Gfx/mtl/mtlPipelineFactory.h"
#include "Gfx/mtl/mtlShaderFactory.h"
#include "Gfx/mtl/mtlTextureFactory.h"
#else
#error "Platform not yet supported!"
#endif

namespace Oryol {
namespace _priv {
#if ORYOL_OPENGL
class meshFactory : public glMeshFactory { };
class pipelineFactory : public glPipelineFactory { };
class shaderFactory : public glShaderFactory { };
class textureFactory : public glTextureFactory { };
#elif ORYOL_D3D11
class meshFactory : public d3d11MeshFactory { };
class pipelineFactory : public d3d11PipelineFactory { };
class shaderFactory : public d3d11ShaderFactory { };
class textureFactory : public d3d11TextureFactory { };
#elif ORYOL_D3D12
class meshFactory : public d3d12MeshFactory { };
class pipelineFactory : public d3d12PipelineFactory { };
class shaderFactory : public d3d12ShaderFactory { };
class textureFactory : public d3d12TextureFactory { };
#elif ORYOL_METAL
class meshFactory : public mtlMeshFactory { };
class pipelineFactory : public mtlPipelineFactory { };
class shaderFactory : public mtlShaderFactory { };
class textureFactory : public mtlTextureFactory { };
#endif

} // namespace _priv
} // namespace Oryol
