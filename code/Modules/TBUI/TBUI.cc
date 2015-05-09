//-----------------------------------------------------------------------------
//  TBUI.cc
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TBUI.h"
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"

namespace Oryol {

TBUI::_state* TBUI::state = nullptr;

//-----------------------------------------------------------------------------
void
TBUI::Setup(const TBUISetup& setup) {
    o_assert_dbg(!IsValid());
    state = Memory::New<_state>();
    state->setup = setup;
    state->resourceContainer.setup(setup);
}

//-----------------------------------------------------------------------------
void
TBUI::Discard() {
    o_assert_dbg(IsValid());
    if (state->ioQueue.IsStarted()) {
        state->ioQueue.Stop();
    }
    state->resourceContainer.discard();
    if (state->mgr.IsValid()) {
        state->mgr.Discard();
    }
    Memory::Delete(state);
    state = nullptr;
}

//-----------------------------------------------------------------------------
void
TBUI::DoAfter(const URL& url, DoAfterFunc doAfterFunc) {
    o_assert_dbg(IsValid());
    
    // FIXME: we should only load resources that are not currently loading
    state->ioQueue.Add(url, [doAfterFunc](const Ptr<Stream>& stream) {
        o_assert_dbg(IsValid());
        state->resourceContainer.add(stream);
        doAfterFunc();
    });
    if (!state->ioQueue.IsStarted()) {
        state->ioQueue.Start();
    }
}

//-----------------------------------------------------------------------------
void
TBUI::DoAfter(const Array<URL>& urls, DoAfterFunc doAfterFunc) {
    o_assert_dbg(IsValid());

    // FIXME: we should only load resources that are not currently loading
    state->ioQueue.AddGroup(urls, [doAfterFunc](const Array<Ptr<Stream>>& streams) {
        o_assert_dbg(IsValid());
        for (const auto& stream : streams) {
            state->resourceContainer.add(stream);
        }
        doAfterFunc();
    });
    if (!state->ioQueue.IsStarted()) {
        state->ioQueue.Start();
    }    
}

//-----------------------------------------------------------------------------
void
TBUI::InitTurboBadger() {
    o_assert_dbg(IsValid());
    state->mgr.Setup(state->setup);
}

//-----------------------------------------------------------------------------
void
TBUI::Draw() {
    o_assert_dbg(IsValid());
    if (state->mgr.IsValid()) {
        state->mgr.Draw();
    }
}

//-----------------------------------------------------------------------------
_priv::tbOryolRootWidget*
TBUI::getRootWidget() {
    o_assert_dbg(IsValid());
    return state->mgr.GetRootWidget();
}

//-----------------------------------------------------------------------------
_priv::tbResourceContainer&
TBUI::resources() {
    o_assert_dbg(IsValid());
    return state->resourceContainer;
}

} // namespace Oryol