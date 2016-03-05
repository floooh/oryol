#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::meshBase
    @ingroup _priv
    @brief private: base class for platform-specific mesh implementations
*/
#include "Resource/Core/resourceBase.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/Attrs/VertexBufferAttrs.h"
#include "Gfx/Attrs/IndexBufferAttrs.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Setup/MeshSetup.h"

namespace Oryol {
namespace _priv {
    
class meshBase : public resourceBase<MeshSetup> {
public:
    /// constructor
    meshBase();
    
    /// vertex buffer attributes
    VertexBufferAttrs vertexBufferAttrs;
    /// index buffer attributes
    IndexBufferAttrs indexBufferAttrs;
    /// number of primitive groups
    int32 numPrimGroups;
    /// primitive groups
    StaticArray<PrimitiveGroup, GfxConfig::MaxNumPrimGroups> primGroups;
    
    /// clear the object
    void Clear();
    /// helper method to check whether mesh config is valid for rendering
    /// (FIXME: find a better place)
    static void checkInputMeshes(meshBase** meshes, int num);
};

} // namespace _priv
} // namespace Oryol


 