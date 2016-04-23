//------------------------------------------------------------------------------
//  tbOryolSystem.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "tb_system.h"
#include "Core/Time/Clock.h"

static tb::TBStr clipboard;

//------------------------------------------------------------------------------
double
tb::TBSystem::GetTimeMS() {
    return Oryol::Clock::Now().Since(0).AsMilliSeconds();
}

//------------------------------------------------------------------------------
void
tb::TBSystem::RescheduleTimer(double fire_time) {
    // FIXME: what is this actually supposed to do?
}

//------------------------------------------------------------------------------
int
tb::TBSystem::GetLongClickDelayMS() {
    return 500;
}

//------------------------------------------------------------------------------
int
tb::TBSystem::GetPanThreshold() {
    return 5 * GetDPI() / 120;
}

//------------------------------------------------------------------------------
int
tb::TBSystem::GetPixelsPerLine() {
    return 40 * GetDPI() / 120;
}

//------------------------------------------------------------------------------
int
tb::TBSystem::GetDPI() {
    // FIXME!
    return 96;
}

//------------------------------------------------------------------------------
void 
tb::TBClipboard::Empty()
{
    clipboard.Clear();
}

//------------------------------------------------------------------------------
bool 
tb::TBClipboard::HasText()
{
    return !clipboard.IsEmpty();
}

//------------------------------------------------------------------------------
bool 
tb::TBClipboard::SetText(const char *text)
{
	return clipboard.Set(text);
}

//------------------------------------------------------------------------------
bool 
tb::TBClipboard::GetText(TBStr &text)
{
	return text.Set(clipboard);
}
