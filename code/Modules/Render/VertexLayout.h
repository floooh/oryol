#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::VertexLayout
    @brief describes the data layout of a vertex in a vertex buffer
    @see VertexComponent
*/
#include "Render/VertexComponent.h"

namespace Oryol {
namespace Render {
    
class VertexLayout {
public:
    /// constructor
    VertexLayout();
    /// clear the object
    void Clear();
    /// return true if layout is empty
    bool Empty() const;
    /// add a component
    void Add(const VertexComponent& comp);
    /// add component by name and format
    void Add(const Core::StringAtom& name, VertexFormat::Code format);
    /// get number of components
    int32 GetNumComponents() const;
    /// read-only get component at index
    const VertexComponent& GetComponent(int32 index) const;
    /// get byte size of vertex
    int32 GetByteSize() const;
    /// get byte offset of a component
    int32 GetComponentByteOffset(int32 componentIndex) const;
    
private:
    /// maximum number of components in layout
    static const int32 MaxNumVertexComponents = 16;

    VertexComponent comps[MaxNumVertexComponents];
    int32 num;
};
    
} // namespace Render
} // namespace Oryol