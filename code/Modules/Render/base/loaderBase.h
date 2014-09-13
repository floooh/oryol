#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::loaderBase
    @ingroup _priv
    @brief resource loader base class for the Render module
*/
#include "Core/RefCounted.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace _priv {
    
class loaderBase : public RefCounted {
public:
    /// resource type of loader (override in subclass)
    virtual ResourceType::Code resourceType() const = 0;
};
    
} // namespace _priv
} // namespace Oryol