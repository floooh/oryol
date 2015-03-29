#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::tbResource
    @brief UI resource wrapper for TBUI module
*/
#include "Core/String/StringAtom.h"
#include "Resource/Core/resourceBase.h"

namespace Oryol {
namespace _priv {

class tbResource : public resourceBase<StringAtom> {
public:
    /// destructor
    ~tbResource();
    
    /// clear the object
    void Clear();
    
    /// stream with loaded resource data
    Ptr<Stream> content;
};

//------------------------------------------------------------------------------
inline
tbResource::~tbResource() {
    o_assert_dbg(!this->content.isValid());
}

//------------------------------------------------------------------------------
inline void
tbResource::Clear() {
    this->content.invalidate();
    resourceBase::Clear();
}

} // namespace _priv
} // namespace Oryol