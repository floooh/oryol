#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::meshBase
    @ingroup _priv
    @brief private: base class for platform-specific mesh implementations
*/
#include "Resource/resourceBase.h"
#include "Gfx/Attrs/VertexBufferAttrs.h"
#include "Gfx/Attrs/IndexBufferAttrs.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Setup/MeshSetup.h"
#include "Gfx/Core/Enums.h"
#include "Core/Assertion.h"
#include "IO/IOProtocol.h"

namespace Oryol {
namespace _priv {
    
class meshBase : public resourceBase<MeshSetup> {
public:
    /// constructor
    meshBase();
    
    /// max number of primitive groups in the mesh
    static const int32 MaxNumPrimGroups = 16;
    
    /// vertex buffer attributes
    VertexBufferAttrs vertexBufferAttrs;
    /// index buffer attributes
    IndexBufferAttrs indexBufferAttrs;
    /// number of primitive groups
    int32 numPrimGroups;
    /// primitive groups (FIXME: replace with StaticArray<>)
    PrimitiveGroup primGroup[MaxNumPrimGroups];
    
    /// clear the object
    void clear();
};

} // namespace _priv
} // namespace Oryol


 