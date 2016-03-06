#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::DrawState
    @brief state required to issue draw calls
    
    The DrawState struct contains state required to issue draw calls
    with the exception of shader uniforms:
    
    - 1 pipeline state object
    - 1..4 mesh objects
    - 0..N textures for the vertex shader stage
    - 0..N textures for the fragment shader stage
*/
#include "Gfx/Core/GfxConfig.h"
#include "Resource/Id.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {

struct DrawState {
    /// the pipeline state object
    Id Pipeline;
    /// input meshes
    StaticArray<Id, GfxConfig::MaxNumInputMeshes> Mesh;
    /// vertex shader stage textures
    StaticArray<Id, GfxConfig::MaxNumTexturesPerStage> VSTexture;
    /// fragment shader stage textures
    StaticArray<Id, GfxConfig::MaxNumTexturesPerStage> FSTexture;
};

} // namepace Oryol
