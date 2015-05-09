#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::MeshSetup
    @ingroup Assets
    @brief setup attributes for meshes
*/
#include "Resource/Id.h"
#include "Resource/Locator.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/VertexLayout.h"
#include "Gfx/Core/PrimitiveGroup.h"

namespace Oryol {
    
class MeshSetup {
public:
    /// asynchronously load from file
    static MeshSetup FromFile(const class Locator& loc, Id placeholder=Id::InvalidId());
    /// setup from from data in memory
    static MeshSetup FromData(Usage::Code vertexUsage=Usage::Immutable, Usage::Code indexUsage=Usage::Immutable);
    /// setup from data in memory with blueprint
    static MeshSetup FromData(const MeshSetup& blueprint);
    /// setup empty mesh (mostly for dynamic streaming)
    static MeshSetup Empty(int32 numVertices, Usage::Code vertexUsage, IndexType::Code indexType=IndexType::None, int32 numIndices=0, Usage::Code indexUsage=Usage::InvalidUsage);
    /// setup a fullscreen quad mesh
    static MeshSetup FullScreenQuad();
    
    /// default constructor
    MeshSetup();
    /// check if should load asynchronously
    bool ShouldSetupFromFile() const;
    /// check if should setup from data in memory
    bool ShouldSetupFromData() const;
    /// check if should setup empty mesh
    bool ShouldSetupEmpty() const;
    /// check if should setup fullscreen quad mesh
    bool ShouldSetupFullScreenQuad() const;
    
    /// vertex-data usage
    Usage::Code VertexUsage;
    /// index-data usage
    Usage::Code IndexUsage;

    /// vertex layout
    VertexLayout Layout;
    
    /// number of vertices (only CreateEmpty)
    int32 NumVertices;
    /// number of indices (only CreateEmpty)
    int32 NumIndices;
    /// index type (only CreateEmpty)
    IndexType::Code IndicesType;

    /// maximum number of primitive groups for one mesh
    static const int32 MaxNumPrimGroups = 8;
    /// add a primitive group (required for CreateEmpty)
    void AddPrimitiveGroup(const PrimitiveGroup& primGroup);
    /// get number of primitive groups
    int32 NumPrimitiveGroups() const;
    /// get primitive group at index
    const class PrimitiveGroup& PrimitiveGroup(int32 index) const;
    
    /// optional instance data mesh
    Id InstanceMesh;
    
    /// resource locator
    class Locator Locator;
    /// placeholder Id
    Id Placeholder;
    
    /// vertex data byte offset in data (default: 0)
    int32 DataVertexOffset;
    /// index data byte offset in data (default: InvalidIndex)
    int32 DataIndexOffset;
    
private:
    int32 numPrimGroups;
    class PrimitiveGroup primGroups[MaxNumPrimGroups];
    bool setupFromFile : 1;
    bool setupFromData : 1;
    bool setupEmpty : 1;
    bool setupFullScreenQuad : 1;
};
    
} // namespace Oryol