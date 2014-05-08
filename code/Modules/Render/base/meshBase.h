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
#include "Render/Core/Enums.h"
#include "Core/Assert.h"
#include "IO/IOProtocol.h"

namespace Oryol {
namespace Render {
    
class meshBase : public Resource::resourceBase<MeshSetup> {
public:
    /// constructor
    meshBase();
    
    /// max number of primitive groups in the mesh
    static const int32 MaxNumPrimitiveGroups = 16;
    
    /// get (optional) IORequest
    const Core::Ptr<IO::IOProtocol::Request>& GetIORequest() const;
    /// get vertex buffer attributes
    const VertexBufferAttrs& GetVertexBufferAttrs() const;
    /// get the index buffer attributes
    const IndexBufferAttrs& GetIndexBufferAttrs() const;
    /// get number of primitive groups
    int32 GetNumPrimitiveGroups() const;
    /// get primitive group at index
    const PrimitiveGroup& GetPrimitiveGroup(int32 index) const;

    /// clear the object
    void clear();
    /// set IO request for asynchronous loading
    void setIORequest(const Core::Ptr<IO::IOProtocol::Request>& ioRequest);
    /// set vertex buffer attrs
    void setVertexBufferAttrs(const VertexBufferAttrs& attrs);
    /// set index buffer attrs
    void setIndexBufferAttrs(const IndexBufferAttrs& attrs);
    /// set number of primitive groups
    void setNumPrimitiveGroups(int32 num);
    /// set primitive group at index
    void setPrimitiveGroup(int32 index, const PrimitiveGroup& group);
    
protected:
    Core::Ptr<IO::IOProtocol::Request> ioRequest;
    VertexBufferAttrs vertexBufferAttrs;
    IndexBufferAttrs indexBufferAttrs;
    int numPrimitiveGroups;
    PrimitiveGroup primitiveGroups[MaxNumPrimitiveGroups];
};

//------------------------------------------------------------------------------
inline const Core::Ptr<IO::IOProtocol::Request>&
meshBase::GetIORequest() const {
    return this->ioRequest;
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


 