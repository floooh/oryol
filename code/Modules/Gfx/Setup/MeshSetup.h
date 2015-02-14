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
#include <limits>

namespace Oryol {
    
class MeshSetup {
public:
    /// asynchronously load from file
    static MeshSetup FromFileAsync(const class Locator& loc, Id placeholder=Id::InvalidId());
    /// setup from from data provided in separate stream object
    static MeshSetup FromStream(Usage::Code vertexUsage=Usage::Immutable, Usage::Code indexUsage=Usage::Immutable);
    /// setup from data with blueprint
    static MeshSetup FromStream(const MeshSetup& blueprint);
    /// setup empty mesh (mostly for dynamic streaming)
    static MeshSetup Empty(int32 numVertices, Usage::Code vertexUsage, IndexType::Code indexType=IndexType::None, int32 numIndices=0, Usage::Code indexUsage=Usage::InvalidUsage);
    /// setup a fullscreen quad mesh
    static MeshSetup FullScreenQuad();
    
    /// default constructor
    MeshSetup();
    /// check if should load asynchronously
    bool ShouldSetupFromFileAsync() const;
    /// check if should setup from stream with file-data
    bool ShouldSetupFromStream() const;
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
    
    /// add a primitive group (required for CreateEmpty)
    void AddPrimitiveGroup(const PrimitiveGroup& primGroup);
    /// get number of primitive groups
    int32 NumPrimitiveGroups() const;
    /// get primitive group at index
    const class PrimitiveGroup& PrimitiveGroup(int32 index) const;
    
    /// optional instance data mesh
    Id InstanceMesh;
    
    /// resource locator (only for LoadAsync)
    class Locator Locator;
    /// placeholder Id (only for LoadAsync)
    Id Placeholder;
    
private:
    static const int32 MaxNumPrimGroups = 8;

    int32 numPrimGroups;
    class PrimitiveGroup primGroups[MaxNumPrimGroups];
    bool setupLoadAsync : 1;
    bool setupFromStream : 1;
    bool setupEmpty : 1;
    bool setupFullScreenQuad : 1;
};
    
} // namespace Oryol