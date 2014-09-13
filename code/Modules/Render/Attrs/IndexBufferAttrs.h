#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IndexBufferAttrs
    @ingroup Render
    @brief attributes of an index buffer
*/
#include "Render/Core/Enums.h"

namespace Oryol {

struct IndexBufferAttrs {
    int32 NumIndices{0};
    IndexType::Code Type{IndexType::InvalidIndexType};
    Usage::Code BufferUsage{Usage::InvalidUsage};
    int32 ByteSize() const {
        return NumIndices * IndexType::ByteSize(Type);
    };
};

} // namespace Oryol