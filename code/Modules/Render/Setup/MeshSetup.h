#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::MeshSetup
    @brief setup attributes for meshes
*/
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Render/Core/Enums.h"
#include "Render/Core/VertexLayout.h"
#include "Render/Core/PrimitiveGroup.h"
#include <limits>

namespace Oryol {
namespace Render {
    
class MeshSetup {
public:
    /// setup from file with creation parameters
    static MeshSetup FromFile(const Resource::Locator& loc, int32 ioLane=0, Usage::Code vertexUsage=Usage::Immutable, Usage::Code indexUsage=Usage::Immutable);
    /// setup from file with blueprint
    static MeshSetup FromFile(const Resource::Locator& loc, const MeshSetup& blueprint);
    /// setup from from data provided in separate stream object
    static MeshSetup FromData(const Resource::Locator& loc, Usage::Code vertexUsage=Usage::Immutable, Usage::Code indexUsage=Usage::Immutable);
    /// setup from data with blueprint
    static MeshSetup FromData(const Resource::Locator& loc, const MeshSetup& blueprint);
    /// setup empty mesh (mostly for dynamic streaming)
    static MeshSetup CreateEmpty(const Resource::Locator& loc, int32 numVertices, Usage::Code vertexUsage, IndexType::Code indexType=IndexType::None, int32 numIndices=0, Usage::Code indexUsage=Usage::InvalidUsage);
    /// setup a fullscreen quad mesh
    static MeshSetup CreateFullScreenQuad(const Resource::Locator& loc);
    
    /// default constructor
    MeshSetup();
    /// check if should setup from file
    bool ShouldSetupFromFile() const;
    /// check if should setup from stream with file-data
    bool ShouldSetupFromData() const;
    /// check if should setup empty mesh
    bool ShouldSetupEmpty() const;
    /// check if should setup fullscreen quad mesh
    bool ShouldSetupFullScreenQuad() const;
    
    /// resource locator
    Resource::Locator Locator;
    /// get vertex-data usage
    Usage::Code VertexUsage{Usage::InvalidUsage};
    /// get index-data usage
    Usage::Code IndexUsage{Usage::InvalidUsage};
    /// get ioLane index
    int32 IOLane;

    /// vertex layout
    VertexLayout Layout;
    
    /// number of vertices (only CreateEmpty)
    int32 NumVertices;
    /// number of indices (only CreateEmpty)
    int32 NumIndices;
    /// index type (only CreateEmpty)
    IndexType::Code IndicesType{IndexType::InvalidIndexType};
    
    /// add a primitive group (required for CreateEmpty)
    void AddPrimitiveGroup(const PrimitiveGroup& primGroup);
    /// get number of primitive groups
    int32 NumPrimitiveGroups() const;
    /// get primitive group at index
    const class PrimitiveGroup& PrimitiveGroup(int32 index) const;
    
    /// optional instance data mesh
    Resource::Id InstanceMesh;
    
private:
    static const int32 MaxNumPrimGroups = 8;

    int32 numPrimGroups;
    class PrimitiveGroup primGroups[MaxNumPrimGroups];
    bool setupFromFile : 1;
    bool setupFromData : 1;
    bool setupEmpty : 1;
    bool setupFullScreenQuad : 1;
};
    
} // namespace Render
} // namespace Oryol