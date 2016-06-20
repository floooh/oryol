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
    /// age of current state in number of frame
    int StateAge = 0;
};

} // namespace Oryol