//------------------------------------------------------------------------------
//  vlkResource.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkResource.h"

namespace Oryol {
namespace _priv {

//==============================================================================
vlkMesh::~vlkMesh() {
    #if ORYOL_DEBUG
    for (const auto& buf : this->buffers) {
        for (const auto& slot : buf.slots) {
            o_assert(!slot.renderBuffer);
            o_assert(!slot.stagingBuffer);
            o_assert(!slot.renderBufferMemory);
            o_assert(!slot.stagingBufferMemory);
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
    vlkMesh::Clear() {
    for (auto& buf : this->buffers) {
        buf = buffer();
    }
    meshBase::Clear();
}

//==============================================================================
vlkShader::~vlkShader() {
    o_assert_dbg(!this->vsModule);
    o_assert_dbg(!this->fsModule);
}

//------------------------------------------------------------------------------
void
vlkShader::Clear() {
    this->vsModule = nullptr;
    this->fsModule = nullptr;
    shaderBase::Clear();
}

} // namespace _priv
} // namespace Oryol

