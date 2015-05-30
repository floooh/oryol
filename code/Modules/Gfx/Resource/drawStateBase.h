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

class programBundle;

class drawStateBase : public resourceBase<DrawStateSetup> {
public:
    /// constructor
    drawStateBase();
    /// destructor
    ~drawStateBase();
    
    /// clear the object
    void Clear();
    
    /// mesh resource Id (cannot be a pointer since meshes might be loaded async)
    class Id msh;
    /// program bundle pointer
    programBundle* prog;
};

} // namespace _priv
} // namespace Oryol
