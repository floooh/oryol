#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::IndexBufferAttrs
    @brief attributes of an index buffer
*/
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {

class IndexBufferAttrs {
public:
    /// number of indices
    int32 NumIndices{0};
    /// index type
    IndexType::Code Type{IndexType::InvalidIndexType};
    /// usage
    Usage::Code BufferUsage{Usage::InvalidUsage};
    /// get byte size (computed)
    int32 ByteSize() const {
        return NumIndices * IndexType::ByteSize(Type);
    };
};

} // namespace Render
} // namespace Oryol