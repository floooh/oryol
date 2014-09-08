#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::loaderBase
    @brief resource loader base class for the Render module
*/
#include "Core/RefCounted.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class loaderBase : public Core::RefCounted {
public:
    /// resource type of loader (override in subclass)
    virtual ResourceType::Code resourceType() const = 0;
};
    
} // namespace Render
} // namespace Oryol