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
class mesh;

class drawStateBase : public resourceBase<DrawStateSetup> {
public:
    /// constructor
    drawStateBase();
    /// destructor
    ~drawStateBase();
    
    /// clear the object
    void Clear();
    
    /// input mesh pointers
    StaticArray<mesh*, DrawStateSetup::MaxInputMeshes> meshes;
    /// program bundle pointer
    programBundle* prog;

};

} // namespace _priv
} // namespace Oryol
