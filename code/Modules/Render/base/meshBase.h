#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::meshBase
    @brief private: base class for platform-specific mesh implementations
*/
#include "Resource/resourceBase.h"
#include "Render/Attrs/VertexBufferAttrs.h"
#include "Render/Attrs/IndexBufferAttrs.h"
#include "Render/Core/PrimitiveGroup.h"
#include "Render/Setup/MeshSetup.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Render {
    
class meshBase : public Resource::resourceBase<MeshSetup> {
public:
    /// constructor
    meshBase();
    
    /// max number of primitive groups in the mesh
    static const int32 MaxNumPrimitiveGroups = 16;
    
    /// return true if the object contains data
    bool IsValid() const;
    /// get vertex buffer attributes
    const VertexBufferAttrs& GetVertexBufferAttrs() const;
    /// get the index buffer attributes
    const IndexBufferAttrs& GetIndexBufferAttrs() const;
    /// get number of primitive groups
    int32 GetNumPrimitiveGroups() const;
    /// get primitive group at index
    const PrimitiveGroup& GetPrimitiveGroup(int32 index) const;
    
protected:
    /// clear the object
    void clear();
    /// set vertex buffer attrs at index
    void setVertexBufferAttrs(const VertexBufferAttrs& attrs);
    /// set index buffer attrs
    void setIndexBufferAttrs(const IndexBufferAttrs& attrs);
    /// set number of primitive groups
    void setNumPrimitiveGroups(int32 num);
    /// set primitive group at index
    void setPrimitiveGroup(int32 index, const PrimitiveGroup& group);

    int32 numVertexBuffers;
    VertexBufferAttrs vertexBufferAttrs;
    IndexBufferAttrs indexBufferAttrs;
    int numPrimitiveGroups;
    PrimitiveGroup primitiveGroups[MaxNumPrimitiveGroups];
};

//------------------------------------------------------------------------------
inline bool
meshBase::IsValid() const {
    return this->numVertexBuffers > 0;
}

//------------------------------------------------------------------------------
inline const VertexBufferAttrs&
meshBase::GetVertexBufferAttrs() const {
    return this->vertexBufferAttrs;
}

//------------------------------------------------------------------------------
inline const IndexBufferAttrs&
meshBase::GetIndexBufferAttrs() const {
    return this->indexBufferAttrs;
}

//------------------------------------------------------------------------------
inline int32
meshBase::GetNumPrimitiveGroups() const {
    return this->numPrimitiveGroups;
}

//------------------------------------------------------------------------------
inline const PrimitiveGroup&
meshBase::GetPrimitiveGroup(int32 index) const {
    o_assert_range(index, MaxNumPrimitiveGroups);
    return this->primitiveGroups[index];
}

} // namespace Render
} // namespace Oryol


 