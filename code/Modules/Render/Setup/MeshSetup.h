#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::MeshSetup
    @brief setup attributes for meshes
*/
#include "Resource/Locator.h"
#include "Render/Core/Enums.h"
#include "Render/Core/VertexLayout.h"
#include "Render/Core/PrimitiveGroup.h"

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
    static MeshSetup CreateEmpty(const Resource::Locator& loc, const VertexLayout& vertexLayout, int32 numVertices, Usage::Code vertexUsage, IndexType::Code indexType=IndexType::None, int32 numIndices=0, Usage::Code indexUsage=Usage::InvalidUsage);
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
    
    /// get the resource locator
    const Resource::Locator& GetLocator() const;
    /// get vertex-data usage
    Usage::Code GetVertexUsage() const;
    /// get index-data usage
    Usage::Code GetIndexUsage() const;
    /// get ioLane index
    int32 GetIOLane() const;
    
    /// get number of vertices (only CreateEmpty)
    int32 GetNumVertices() const;
    /// get number of indices (only CreateEmpty)
    int32 GetNumIndices() const;
    /// get VertexLayout (only CreateEmpty)
    const VertexLayout& GetVertexLayout() const;
    /// get IndexType (only CreateEmpty)
    IndexType::Code GetIndexType() const;
    
    /// add a primitive group (required for CreateEmpty)
    void AddPrimitiveGroup(const PrimitiveGroup& primGroup);
    /// get number of primitive groups
    int32 GetNumPrimitiveGroups() const;
    /// get primitive group at index
    const PrimitiveGroup& GetPrimitiveGroup(int32 index) const;

private:
    static const int32 MaxNumPrimGroups = 8;

    Resource::Locator locator;
    Usage::Code vertexUsage;
    Usage::Code indexUsage;
    int32 ioLane;
    int32 numVertices;
    int32 numIndices;
    IndexType::Code indexType;
    VertexLayout vertexLayout;
    int32 numPrimGroups;
    PrimitiveGroup primGroups[MaxNumPrimGroups];
    bool setupFromFile : 1;
    bool setupFromData : 1;
    bool setupEmpty : 1;
    bool setupFullScreenQuad : 1;
};
    
} // namespace Render
} // namespace Oryol