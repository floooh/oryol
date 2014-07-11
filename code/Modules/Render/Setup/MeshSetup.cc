//------------------------------------------------------------------------------
//  MeshSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshSetup.h"

namespace Oryol {
namespace Render {

using namespace Resource;
    
//------------------------------------------------------------------------------
MeshSetup::MeshSetup() :
vertexUsage(Usage::InvalidUsage),
indexUsage(Usage::InvalidUsage),
ioLane(0),
numVertices(0),
numIndices(0),
indexType(IndexType::None),
numPrimGroups(0),
setupFromFile(false),
setupFromData(false),
setupEmpty(false),
setupFullScreenQuad(false) {
    // empty
}

//------------------------------------------------------------------------------
MeshSetup
MeshSetup::FromFile(const Locator& loc, int32 ioLane, Usage::Code vbUsage, Usage::Code ibUsage) {
    MeshSetup setup;
    setup.locator = loc;
    setup.ioLane = ioLane;
    setup.vertexUsage = vbUsage;
    setup.indexUsage = ibUsage;
    setup.setupFromFile = true;
    return setup;
}

//------------------------------------------------------------------------------
MeshSetup
MeshSetup::FromFile(const Locator& loc, const MeshSetup& blueprint) {
    MeshSetup setup(blueprint);
    setup.locator = loc;
    setup.setupFromFile = true;
    return setup;
}

//------------------------------------------------------------------------------
MeshSetup
MeshSetup::FromData(const Locator& loc, Usage::Code vbUsage, Usage::Code ibUsage) {
    MeshSetup setup;
    setup.locator = loc;
    setup.vertexUsage = vbUsage;
    setup.indexUsage = ibUsage;
    setup.setupFromData = true;
    return setup;
}

//------------------------------------------------------------------------------
MeshSetup
MeshSetup::FromData(const Locator& loc, const MeshSetup& blueprint) {
    MeshSetup setup(blueprint);
    setup.locator = loc;
    setup.setupFromData = true;
    return setup;
}

//------------------------------------------------------------------------------
MeshSetup
MeshSetup::CreateEmpty(const Locator& loc,
                       int32 numVertices,
                       Usage::Code vertexUsage,
                       IndexType::Code indexType,
                       int32 numIndices,
                       Usage::Code indexUsage) {
    
    o_assert(numVertices > 0);
    
    MeshSetup setup;
    setup.locator = loc;
    setup.setupEmpty = true;
    setup.vertexUsage = vertexUsage;
    setup.indexUsage = indexUsage;
    setup.numVertices = numVertices;
    setup.numIndices = numIndices;
    setup.indexType = indexType;
    return setup;
}

//------------------------------------------------------------------------------
MeshSetup
MeshSetup::CreateFullScreenQuad(const Locator& loc) {
    MeshSetup setup;
    setup.locator = loc;
    setup.setupFullScreenQuad = true;
    return setup;
}

//------------------------------------------------------------------------------
bool
MeshSetup::ShouldSetupFromFile() const {
    return this->setupFromFile;
}

//------------------------------------------------------------------------------
bool
MeshSetup::ShouldSetupFromData() const {
    return this->setupFromData;
}

//------------------------------------------------------------------------------
bool
MeshSetup::ShouldSetupEmpty() const {
    return this->setupEmpty;
}

//------------------------------------------------------------------------------
bool
MeshSetup::ShouldSetupFullScreenQuad() const {
    return this->setupFullScreenQuad;
}

//------------------------------------------------------------------------------
const Locator&
MeshSetup::GetLocator() const {
    return this->locator;
}

//------------------------------------------------------------------------------
Usage::Code
MeshSetup::GetVertexUsage() const {
    return this->vertexUsage;
}

//------------------------------------------------------------------------------
Usage::Code
MeshSetup::GetIndexUsage() const {
    return this->indexUsage;
}

//------------------------------------------------------------------------------
int32
MeshSetup::GetIOLane() const {
    return this->ioLane;
}

//------------------------------------------------------------------------------
int32
MeshSetup::GetNumVertices() const {
    return this->numVertices;
}

//------------------------------------------------------------------------------
int32
MeshSetup::GetNumIndices() const {
    return this->numIndices;
}

//------------------------------------------------------------------------------
VertexLayout&
MeshSetup::VertexLayout() {
    return this->vertexLayout;
}

//------------------------------------------------------------------------------
const VertexLayout&
MeshSetup::VertexLayout() const {
    return this->vertexLayout;
}

//------------------------------------------------------------------------------
IndexType::Code
MeshSetup::GetIndexType() const {
    return this->indexType;
}

//------------------------------------------------------------------------------
void
MeshSetup::AddPrimitiveGroup(const PrimitiveGroup& primGroup) {
    o_assert(this->setupEmpty);
    o_assert(this->numPrimGroups < MaxNumPrimGroups);
    this->primGroups[this->numPrimGroups++] = primGroup;
}

//------------------------------------------------------------------------------
int32
MeshSetup::GetNumPrimitiveGroups() const {
    return this->numPrimGroups;
}

//------------------------------------------------------------------------------
const PrimitiveGroup&
MeshSetup::GetPrimitiveGroup(int32 index) const {
    o_assert_range(index, MaxNumPrimGroups);
    return this->primGroups[index];
}

//------------------------------------------------------------------------------
void
MeshSetup::SetInstanceMesh(const Resource::Id& msh) {
    o_assert(msh.IsValid());
    this->instMesh = msh;
}

//------------------------------------------------------------------------------
const Resource::Id&
MeshSetup::GetInstanceMesh() const {
    return this->instMesh;
}

} // namespace Render
} // namespace Oryol