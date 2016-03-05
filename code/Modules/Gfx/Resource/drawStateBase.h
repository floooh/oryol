#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawStateBase
    @ingroup _priv
    @brief base class for drawState implementations
*/
#include "Resource/Core/resourceBase.h"
#include "Gfx/Setup/DrawStateSetup.h"

namespace Oryol {
namespace _priv {

class shader;
class mesh;

class drawStateBase : public resourceBase<DrawStateSetup> {
public:
    /// destructor
    ~drawStateBase();
    /// clear the object
    void Clear();    
    /// shader pointer
    shader* shd = nullptr;
};

} // namespace _priv
} // namespace Oryol
