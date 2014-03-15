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
setupFromFile(false),
setupFromData(false) {
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
MeshSetup::FromData(const Locator& loc, Usage::Code vbUsage, Usage::Code ibUsage) {
    MeshSetup setup;
    setup.locator = loc;
    setup.vertexUsage = vbUsage;
    setup.indexUsage = ibUsage;
    setup.setupFromData = true;
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

} // namespace Render
} // namespace Oryol