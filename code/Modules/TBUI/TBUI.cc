//-----------------------------------------------------------------------------
//  TBUI.cc
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TBUI.h"
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"
#include "IO/IO.h"

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
    IO::Load(url, [doAfterFunc](IO::LoadResult ioResult) {
        o_assert_dbg(IsValid());
        state->resourceContainer.add(ioResult.Url.Get(), std::move(ioResult.Data));
        doAfterFunc();
    });
}

//-----------------------------------------------------------------------------
void
TBUI::DoAfter(const Array<URL>& urls, DoAfterFunc doAfterFunc) {
    o_assert_dbg(IsValid());

    // FIXME: we should only load resources that are not currently loading
    IO::LoadGroup(urls, [doAfterFunc](Array<IO::LoadResult> ioResults) {
        o_assert_dbg(IsValid());
        for (auto& res : ioResults) {
            state->resourceContainer.add(res.Url.Get(), std::move(res.Data));
        }
        doAfterFunc();
    });
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
