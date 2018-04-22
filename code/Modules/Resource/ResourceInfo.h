#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceInfo
    @ingroup Resource
    @brief queryable information about a resource
*/
#include "Resource/ResourceState.h"

namespace Oryol {

class ResourceInfo {
public:
    /// current state of the resource
    ResourceState::Code State = ResourceState::InvalidState;
};

} // namespace Oryol