#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::StateBlockSetup
    @brief setup attributes for StateBlocks
*/
#include "Resource/Locator.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class StateBlockSetup {
public:
    /// default constructor
    StateBlockSetup();
    /// construct with resource locator
    StateBlockSetup(const Resource::Locator& loc);
    
    /// add state
    void AddState(State::Code state, bool b0);
    /// add state
    void AddState(State::Code state, bool b0, bool b1, bool b2, bool b3);
    /// add state
    void AddState(State::Code state, State::Value v0);
    /// add state
    void AddState(State::Code state, State::Value v0, State::Value v1);
    /// add state
    void AddState(State::Code state, State::Value v0, State::Value v1, State::Value v2);
    /// add state
    void AddState(State::Code state, State::Value v0, State::Value v1, State::Value v2, State::Value v3);
    /// add state
    void AddState(State::Code state, float32 f0);
    /// add state
    void AddState(State::Code state, float32 f0, float32 f1);
    /// add state
    void AddState(State::Code state, float32 f0, float32 f1, float32 f2, float32 f3);
    /// add state
    void AddState(State::Code state, int32 i0);
    /// add state
    void AddState(State::Code state, int32 i0, int32 i1);
    /// add state
    void AddState(State::Code state, int32 i0, int32 i1, int32 i2, int32 i3);
    /// add state
    void AddState(State::Code state, State::Value v0, int32 i0);
    /// add state
    void AddState(State::Code state, State::Value v0, int32 i0, int32 i1);
    /// add state
    void AddState(State::Code state, State::Value v0, State::Value v1, int32 i0, int32 i1);
    
    /// get the resource locator
    const Resource::Locator& GetLocator() const;
    /// get number of states in the state block
    int32 GetNumStates() const;
    /// get pointer to first state
    const State::Object* GetStates() const;
    
private:
    /// grab a new state object and partially fill it
    State::Object& grabState(State::Code state, State::Signature sig);

    static const int32 MaxNumStates = 12;
    Resource::Locator locator;
    int32 numStates;
    State::Object states[MaxNumStates];
};

//------------------------------------------------------------------------------
inline const Resource::Locator&
StateBlockSetup::GetLocator() const {
    return this->locator;
}

//------------------------------------------------------------------------------
inline int32
StateBlockSetup::GetNumStates() const {
    return this->numStates;
}

//------------------------------------------------------------------------------
inline const State::Object*
StateBlockSetup::GetStates() const {
    return &(this->states[0]);
}

} // namespace Render
} // namespace Oryol