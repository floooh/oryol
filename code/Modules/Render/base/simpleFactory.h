#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::simpleFactory
    @brief template factory base class for simple rendering resources
    
    This base factory can be used as-is for very simple resource which
    don't require a customized setup action at all.
*/
#include "Resource/simpleFactory.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
template<class RESOURCE, ResourceType::Code TYPE>
class simpleFactory : public Resource::simpleFactory<RESOURCE> {
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
simpleFactory<RESOURCE,TYPE>::GetResourceType() const {
    return TYPE;
}

//------------------------------------------------------------------------------
template<class RESOURCE, ResourceType::Code TYPE> void
simpleFactory<RESOURCE,TYPE>::SetupResource(RESOURCE& res) {
    o_assert(res.GetState() == Resource::State::Setup);
    res.setState(Resource::State::Valid);
}

//------------------------------------------------------------------------------
template<class RESOURCE, ResourceType::Code TYPE> void
simpleFactory<RESOURCE,TYPE>::DestroyResource(RESOURCE& res) {
    o_assert(res.GetState() == Resource::State::Valid);
    res.setState(Resource::State::Setup);
}

} // namespace Render
} // namespace Oryol
