#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::simpleRenderFactory
    @ingroup _priv
    @brief template factory base class for simple rendering resources
    
    This base factory can be used as-is for very simple resource which
    don't require a customized setup action at all.
*/
#include "Resource/simpleFactory.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace _priv {
    
template<class RESOURCE, ResourceType::Code TYPE>
class renderSimpleFactory : public simpleFactory<RESOURCE> {
public:
    /// get the resource type this factory produces
    uint16 GetResourceType() const;
    /// setup shader resource
    void SetupResource(RESOURCE& res);
    /// destroy the shader
    void DestroyResource(RESOURCE& res);
};

//------------------------------------------------------------------------------
template<class RESOURCE, ResourceType::Code TYPE> uint16
renderSimpleFactory<RESOURCE,TYPE>::GetResourceType() const {
    return TYPE;
}

//------------------------------------------------------------------------------
template<class RESOURCE, ResourceType::Code TYPE> void
renderSimpleFactory<RESOURCE,TYPE>::SetupResource(RESOURCE& res) {
    o_assert(res.GetState() == ResourceState::Setup);
    res.setState(ResourceState::Valid);
}

//------------------------------------------------------------------------------
template<class RESOURCE, ResourceType::Code TYPE> void
renderSimpleFactory<RESOURCE,TYPE>::DestroyResource(RESOURCE& res) {
    o_assert(res.GetState() == ResourceState::Valid);
    res.setState(ResourceState::Setup);
}

} // namespace _priv
} // namespace Oryol
