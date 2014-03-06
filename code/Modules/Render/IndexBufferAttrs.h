#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::IndexBufferAttrs
    @brief attributes of an index buffer
*/
#include "Render/IndexType.h"
#include "Render/Usage.h"

namespace Oryol {
namespace Render {

class IndexBufferAttrs {
public:
    /// constructor
    IndexBufferAttrs();
    
    /// get number of indices
    int32 GetNumIndices() const;
    /// get index type (16- or 32-bit)
    IndexType::Code GetIndexType() const;
    /// get usage
    Usage::Code GetUsage() const;
    /// get byte size of index data
    int32 GetByteSize() const;
    
    /// set number of indices
    void setNumIndices(int32 num);
    /// set index type
    void setIndexType(IndexType::Code t);
    /// set usage
    void setUsage(Usage::Code usg);

private:
    int32 numIndices;
    IndexType::Code indexType;
    Usage::Code usage;
};

//------------------------------------------------------------------------------
inline int32
IndexBufferAttrs::GetNumIndices() const {
    return this->numIndices;
}

//------------------------------------------------------------------------------
inline IndexType::Code
IndexBufferAttrs::GetIndexType() const {
    return this->indexType;
}

//------------------------------------------------------------------------------
inline Usage::Code
IndexBufferAttrs::GetUsage() const {
    return this->usage;
}

//------------------------------------------------------------------------------
inline int32
IndexBufferAttrs::GetByteSize() const {
    return this->numIndices * IndexType::ByteSize(this->indexType);
}

} // namespace Render
} // namespace Oryol