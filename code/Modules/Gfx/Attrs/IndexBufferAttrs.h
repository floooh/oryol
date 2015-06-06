#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IndexBufferAttrs
    @ingroup Gfx
    @brief attributes of an index buffer
*/
#include "Gfx/Core/Enums.h"

namespace Oryol {

struct IndexBufferAttrs {
    /// number of indices in the index buffer
    int32 NumIndices{0};
    /// type of indices (16-bit or 32-bit)
    IndexType::Code Type{IndexType::InvalidIndexType};
    /// buffer usage hint
    Usage::Code BufferUsage{Usage::InvalidUsage};
    /// computes the byte size of index buffer data
    int32 ByteSize() const {
        return NumIndices * IndexType::ByteSize(Type);
    };
};

} // namespace Oryol