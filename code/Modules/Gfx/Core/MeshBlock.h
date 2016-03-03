#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::MeshBlock
    @brief a group of input meshes for rendering used in Gfx::ApplyDrawState()
*/
#include "Core/Containers/StaticArray.h"
#include "Gfx/Core/GfxConfig.h"
#include "Resource/Id.h"

namespace Oryol {

typedef StaticArray<Id, GfxConfig::MaxNumInputMeshes> MeshBlock;

} // namespace Oryol
