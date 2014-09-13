//------------------------------------------------------------------------------
//  StateTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Resource/ResourceState.h"
#include "Core/String/String.h"

using namespace Oryol;

TEST(StateTest) {
    CHECK(ResourceState::NumStates == 5);
    CHECK(ResourceState::FromString("Initial") == ResourceState::Initial);
    CHECK(ResourceState::FromString("Setup") == ResourceState::Setup);
    CHECK(ResourceState::FromString("Pending") == ResourceState::Pending);
    CHECK(ResourceState::FromString("Valid") == ResourceState::Valid);
    CHECK(ResourceState::FromString("Failed") == ResourceState::Failed);
    CHECK(String(ResourceState::ToString(ResourceState::Initial)) == "Initial");
    CHECK(String(ResourceState::ToString(ResourceState::Setup)) == "Setup");
    CHECK(String(ResourceState::ToString(ResourceState::Pending)) == "Pending");
    CHECK(String(ResourceState::ToString(ResourceState::Valid)) == "Valid");
    CHECK(String(ResourceState::ToString(ResourceState::Failed)) == "Failed");
}

