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
    int numPrimGroups;
    /// primitive groups
    StaticArray<PrimitiveGroup, GfxConfig::MaxNumPrimGroups> primGroups;
    
    /// clear the object
    void Clear();
};

} // namespace _priv
} // namespace Oryol


 