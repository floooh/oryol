#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::MeshSetup
    @brief setup attributes for meshes
*/
#include "Resource/Locator.h"
#include "Render/Core/Enums.h"

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
    
    /// default constructor
    MeshSetup();
    /// check if should setup from file
    bool ShouldSetupFromFile() const;
    /// check if should setup from stream with file-data
    bool ShouldSetupFromData() const;
    
    /// get the resource locator
    const Resource::Locator& GetLocator() const;
    /// set vertex-data usage
    void SetVertexUsage(Usage::Code usg);
    /// get vertex-data usage
    Usage::Code GetVertexUsage() const;
    /// set index-data usage
    void SetIndexUsage(Usage::Code usg);
    /// get index-data usage
    Usage::Code GetIndexUsage() const;
    /// set ioLane index
    void SetIOLane(int32 lane);
    /// get ioLane index
    int32 GetIOLane() const;
    
private:
    Resource::Locator locator;
    Usage::Code vertexUsage;
    Usage::Code indexUsage;
    int32 ioLane;
    bool setupFromFile : 1;
    bool setupFromData : 1;
};
    
} // namespace Render
} // namespace Oryol