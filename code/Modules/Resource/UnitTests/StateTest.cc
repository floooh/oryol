//------------------------------------------------------------------------------
//  StateTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Resource/State.h"
#include "Core/String/String.h"

using namespace Oryol;
using namespace Oryol::Resource;

TEST(StateTest) {
    CHECK(State::NumStates == 5);
    CHECK(State::FromString("Initial") == State::Initial);
    CHECK(State::FromString("Setup") == State::Setup);
    CHECK(State::FromString("Pending") == State::Pending);
    CHECK(State::FromString("Valid") == State::Valid);
    CHECK(State::FromString("Failed") == State::Failed);
    CHECK(String(State::ToString(State::Initial)) == "Initial");
    CHECK(String(State::ToString(State::Setup)) == "Setup");
    CHECK(String(State::ToString(State::Pending)) == "Pending");
    CHECK(String(State::ToString(State::Valid)) == "Valid");
    CHECK(String(State::ToString(State::Failed)) == "Failed");
}

