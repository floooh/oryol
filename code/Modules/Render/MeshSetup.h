#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::MeshSetup
    @brief setup attributes for meshes
*/
#include "Resource/Locator.h"
#include "Render/Usage.h"

namespace Oryol {
namespace Render {
    
class MeshSetup {
public:
    /// setup from file
    static MeshSetup FromFile(const Resource::Locator& loc, int32 ioLane=0, Usage::Code vertexUsage=Usage::Immutable, Usage::Code indexUsage=Usage::Immutable);
    /// setup from from data provided in stream object
    static MeshSetup FromData(const Resource::Locator& loc, Usage::Code vertexUsage=Usage::Immutable, Usage::Code indexUsage=Usage::Immutable);
    
    /// default constructor
    MeshSetup();
    /// check if should setup from file
    bool ShouldSetupFromFile() const;
    /// check if should setup from stream with file-data
    bool ShouldSetupFromData() const;
    
    /// get the resource locator
    const Resource::Locator& GetLocator() const;
    /// get vertex-data usage
    Usage::Code GetVertexUsage() const;
    /// get index-data usage
    Usage::Code GetIndexUsage() const;
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