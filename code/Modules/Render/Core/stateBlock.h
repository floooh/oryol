#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::stateBlock
    @brief capture multiple render state switches under a single handle
    
    NOTE: the actual state information is stored in the embedded setup
    object, and not duplicated in the resource object itself.
*/
#include "Resource/resourceBase.h"
#include "Render/Setup/StateBlockSetup.h"

namespace Oryol {
namespace Render {
    
class stateBlock : public Resource::resourceBase<StateBlockSetup> {
public:
    /// clear the object
    void clear();
    /// get number of states in the state block
    int32 GetNumStates() const;
    /// get pointer to start of state array
    const State::Object* GetStates() const;
};

//------------------------------------------------------------------------------
inline int32
stateBlock::GetNumStates() const {
    return this->setup.GetNumStates();
}

//------------------------------------------------------------------------------
inline const State::Object*
stateBlock::GetStates() const {
    return this->setup.GetStates();
}

} // namespace Render
} // namespace Oryol
