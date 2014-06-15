//------------------------------------------------------------------------------
//  StateBlockFactoryTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Core/stateBlockFactory.h"
#include "Render/Core/Enums.h"

using namespace Oryol;
using namespace Oryol::Render;

static void check(const State::Object* stateObj) {
    CHECK(stateObj[0].state == State::FrontFace);
    CHECK(stateObj[0].sig == State::V0);
    CHECK(stateObj[0].vec.val[0].v == State::CW);
    CHECK(stateObj[1].state == State::CullFaceEnabled);
    CHECK(stateObj[1].sig == State::B0);
    CHECK(stateObj[1].vec.val[0].b == true);
    CHECK(stateObj[2].state == State::DepthOffset);
    CHECK(stateObj[2].sig == State::F0_F1);
    CHECK_CLOSE(0.1f, stateObj[2].vec.val[0].f, 0.0001f);
    CHECK_CLOSE(0.2f, stateObj[2].vec.val[1].f, 0.0001f);
    CHECK(stateObj[3].state == State::ScissorRect);
    CHECK(stateObj[3].sig == State::I0_I1_I2_I3);
    CHECK(stateObj[3].vec.val[0].i == 1);
    CHECK(stateObj[3].vec.val[1].i == 2);
    CHECK(stateObj[3].vec.val[2].i == 3);
    CHECK(stateObj[3].vec.val[3].i == 4);
    CHECK(stateObj[4].state == State::BlendColor);
    CHECK(stateObj[4].sig == State::F0_F1_F2_F3);
    CHECK_CLOSE(0.1f, stateObj[4].vec.val[0].f, 0.0001f);
    CHECK_CLOSE(0.2f, stateObj[4].vec.val[1].f, 0.0001f);
    CHECK_CLOSE(0.3f, stateObj[4].vec.val[2].f, 0.0001f);
    CHECK_CLOSE(0.4f, stateObj[4].vec.val[3].f, 0.0001f);
}

TEST(StateBlockFactoryTest) {
    
    stateBlockFactory sbFactory;
    sbFactory.Setup();
    
    StateBlockSetup sbSetup("sb");
    sbSetup.AddState(State::FrontFace, State::CW);
    sbSetup.AddState(State::CullFaceEnabled, true);
    sbSetup.AddState(State::DepthOffset, 0.1f, 0.2f);
    sbSetup.AddState(State::ScissorRect, 1, 2, 3, 4);
    sbSetup.AddState(State::BlendColor, 0.1f, 0.2f, 0.3f, 0.4f);
    CHECK(sbSetup.GetNumStates() == 6);
    check(sbSetup.GetStates());
    
    stateBlock sb;
    sb.setSetup(sbSetup);
    sbFactory.SetupResource(sb);
    CHECK(sb.GetState() == Resource::State::Valid);
    CHECK(sb.GetNumStates() == 5);
    check(sb.GetStates());
    sbFactory.DestroyResource(sb);
    CHECK(sb.GetState() == Resource::State::Setup);
    sbFactory.Discard();    
}