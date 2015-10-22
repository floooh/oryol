#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceLoader
    @ingroup Resource
    @brief base class for resource loaders
*/
#include "Core/RefCounted.h"
#include "Resource/Id.h"
#include "Resource/Locator.h"
#include "Resource/ResourceState.h"

namespace Oryol {

class ResourceLoader : public RefCounted {
    OryolClassDecl(ResourceLoader);
public:
    /// return resource locator
    virtual class Locator Locator() const;
    /// start loading, return a resource id
    virtual Id Start();
    /// continue loading, return resource state (Pending, Valid, Failed)
    virtual ResourceState::Code Continue();
    /// cancel the resource loading process
    virtual void Cancel();
};

} // namespace Oryol
