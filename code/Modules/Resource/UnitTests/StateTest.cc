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
    CHECK(String(ResourceState::ToString(ResourceState::Initial)) == "Initial");
    CHECK(String(ResourceState::ToString(ResourceState::Setup)) == "Setup");
    CHECK(String(ResourceState::ToString(ResourceState::Pending)) == "Pending");
    CHECK(String(ResourceState::ToString(ResourceState::Valid)) == "Valid");
    CHECK(String(ResourceState::ToString(ResourceState::Failed)) == "Failed");
}

