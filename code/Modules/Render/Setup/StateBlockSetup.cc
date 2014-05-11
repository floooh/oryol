//------------------------------------------------------------------------------
//  StateBlockSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "StateBlockSetup.h"

namespace Oryol {
namespace Render {

using namespace Resource;
    
//------------------------------------------------------------------------------
StateBlockSetup::StateBlockSetup() :
numStates(0) {
    // empty
}

//------------------------------------------------------------------------------
StateBlockSetup::StateBlockSetup(const Locator& l) :
locator(l),
numStates(0) {
    // empty
}

//------------------------------------------------------------------------------
State::Object&
StateBlockSetup::grabState(State::Code state, State::Signature sig) {
    o_assert(this->numStates < MaxNumStates);
    State::Object& obj = this->states[this->numStates++];
    obj.state = state;
    obj.sig = sig;
    return obj;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, bool b0) {
    State::Object& obj = this->grabState(state, State::B0);
    obj.vec.val[0].b = b0;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, bool b0, bool b1, bool b2, bool b3) {
    State::Object& obj = this->grabState(state, State::B0_B1_B2_B3);
    obj.vec.val[0].b = b0;
    obj.vec.val[1].b = b1;
    obj.vec.val[2].b = b2;
    obj.vec.val[3].b = b3;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, State::Value v0) {
    State::Object& obj = this->grabState(state, State::V0);
    obj.vec.val[0].v = v0;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, State::Value v0, State::Value v1) {
    State::Object& obj = this->grabState(state, State::V0_V1);
    obj.vec.val[0].v = v0;
    obj.vec.val[1].v = v1;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, State::Value v0, State::Value v1, State::Value v2) {
    State::Object& obj = this->grabState(state, State::V0_V1_V2);
    obj.vec.val[0].v = v0;
    obj.vec.val[1].v = v1;
    obj.vec.val[2].v = v2;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, State::Value v0, State::Value v1, State::Value v2, State::Value v3) {
    State::Object& obj = this->grabState(state, State::V0_V1_V2_V3);
    obj.vec.val[0].v = v0;
    obj.vec.val[1].v = v1;
    obj.vec.val[2].v = v2;
    obj.vec.val[3].v = v3;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, float32 f0) {
    State::Object& obj = this->grabState(state, State::F0);
    obj.vec.val[0].f = f0;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, float32 f0, float32 f1) {
    State::Object& obj = this->grabState(state, State::F0_F1);
    obj.vec.val[0].f = f0;
    obj.vec.val[1].f = f1;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, float32 f0, float32 f1, float32 f2, float32 f3) {
    State::Object& obj = this->grabState(state, State::F0_F1_F2_F3);
    obj.vec.val[0].f = f0;
    obj.vec.val[1].f = f1;
    obj.vec.val[2].f = f2;
    obj.vec.val[3].f = f3;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, int32 i0) {
    State::Object& obj = this->grabState(state, State::I0);
    obj.vec.val[0].i = i0;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, int32 i0, int32 i1) {
    State::Object& obj = this->grabState(state, State::I0_I1);
    obj.vec.val[0].i = i0;
    obj.vec.val[1].i = i1;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, int32 i0, int32 i1, int32 i2, int32 i3) {
    State::Object& obj = this->grabState(state, State::I0_I1_I2_I3);
    obj.vec.val[0].i = i0;
    obj.vec.val[1].i = i1;
    obj.vec.val[2].i = i2;
    obj.vec.val[3].i = i3;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, State::Value v0, int32 i0) {
    State::Object& obj = this->grabState(state, State::V0_I0);
    obj.vec.val[0].v = v0;
    obj.vec.val[1].i = i0;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, State::Value v0, int32 i0, int32 i1) {
    State::Object& obj = this->grabState(state, State::V0_I0_I1);
    obj.vec.val[0].v = v0;
    obj.vec.val[1].i = i0;
    obj.vec.val[2].i = i1;
}

//------------------------------------------------------------------------------
void
StateBlockSetup::AddState(State::Code state, State::Value v0, State::Value v1, int32 i0, int32 i1) {
    State::Object& obj = this->grabState(state, State::V0_V1_I0_I1);
    obj.vec.val[0].v = v0;
    obj.vec.val[1].v = v1;
    obj.vec.val[2].i = i0;
    obj.vec.val[3].i = i1;
}

} // namespace Render
} // namespace Oryol