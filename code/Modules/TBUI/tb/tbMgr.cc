//-----------------------------------------------------------------------------
//  tbMgr.cc
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "tbMgr.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "IO/Core/URLBuilder.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "animation/tb_widget_animation.h"
#include "tb_font_renderer.h"
#include "tb_language.h"

void register_tbbf_font_renderer();

using namespace tb;

namespace Oryol {
namespace _priv {

//-----------------------------------------------------------------------------
tbMgr::~tbMgr() {
    o_assert(!this->IsValid());
}

//-----------------------------------------------------------------------------
void
tbMgr::Setup(const TBUISetup& setup) {
    o_assert_dbg(!this->IsValid());
    o_assert_dbg(setup.DefaultSkin.IsValid());
    o_assert_dbg(!setup.Fonts.Empty());
    o_assert_dbg(setup.DefaultFontName.IsValid());
    
    // initialize turbobadger
    this->renderer = Memory::New<tbOryolBatchRenderer>();
    this->renderer->Setup();
    tb_core_init(this->renderer);
    g_tb_lng->Load(setup.Locale.AsCStr());
    
    // initialize turbobadger skin
    if (!g_tb_skin->Load(setup.DefaultSkin.AsCStr(), setup.OverrideSkin.AsCStr())) {
        o_error("tbMgr::Setup(): Failed to load skins!");
    }
    
    // use turbobadger's built-in font renderer
    register_tbbf_font_renderer();
    
    // setup fonts
    for (const auto& font : setup.Fonts) {
        g_font_manager->AddFontInfo(font.Location.AsCStr(), font.Name.AsCStr());
    }
    TBFontDescription fontDesc;
    fontDesc.SetID(TBIDC(setup.DefaultFontName.AsCStr()));
    fontDesc.SetSize(g_tb_skin->GetDimensionConverter()->DpToPx(setup.DefaultFontSize));
    g_font_manager->SetDefaultFontDescription(fontDesc);
    TBFontFace* font = g_font_manager->CreateFontFace(g_font_manager->GetDefaultFontDescription());
    if (font) {
        font->RenderGlyphs(setup.GlyphSet.AsCStr());
    }

    // input event handlers
    Input::SubscribeMouse([this](const Mouse::Event& e) {
        switch (e.Type) {
            case Mouse::Event::Move:
                this->onMouseMove((int)e.Position.x, (int)e.Position.y);
                break;
            case Mouse::Event::ButtonDown:
                this->onMouseButton(e.Button, true);
                break;
            case Mouse::Event::ButtonUp:
                this->onMouseButton(e.Button, false);
                break;
            case Mouse::Event::Scroll:
                this->onScroll((int)e.ScrollMovement.x, (int)e.ScrollMovement.y);
                break;
            default:
                break;
        }
    });
    Input::SubscribeKeyboard([this](const Keyboard::Event& e) {
        switch (e.Type) {
            case Keyboard::Event::KeyDown:
            case Keyboard::Event::KeyRepeat:
                this->onKey(e.KeyCode, true, false);
                break;
            case Keyboard::Event::KeyUp:
                this->onKey(e.KeyCode, false, true);
                break;
            case Keyboard::Event::WChar:
                this->onWChar(e.WCharCode);
                break;
            default:
                break;
        }
    });
    Input::SubscribeTouchpad([this](const Touchpad::Event& e) {
        switch (e.Type) {
            case Touchpad::Event::Tapped:
                this->onTapped(e.Position[0]);
                break;
            case Touchpad::Event::PanningStarted:
                this->onPanningStarted(e.StartPosition[0]);
                break;
            case Touchpad::Event::Panning:
                this->onPanning(e.Position[0]);
                break;
            case Touchpad::Event::PanningEnded:
                this->onPanningEnded(e.Position[0]);
                break;
            default:
                break;
        }
    });

    // add per-frame update method
    this->runLoopId = Core::PostRunLoop()->Add([this] {
        this->update();
    });
    
    TBWidgetsAnimationManager::Init();
    
    this->isValid = true;
}

//-----------------------------------------------------------------------------
void
tbMgr::Discard() {
    o_assert_dbg(this->IsValid());
    Core::PostRunLoop()->Remove(this->runLoopId);
    
    TBWidgetsAnimationManager::Shutdown();

    tb_core_shutdown();

    this->renderer->Discard();
    Memory::Delete(this->renderer);
        
    this->isValid = false;
}

//-----------------------------------------------------------------------------
bool
tbMgr::IsValid() const {
    return this->isValid;
}

//-----------------------------------------------------------------------------
void
tbMgr::update() {
    this->rootWidget.InvokeProcessStates();
    this->rootWidget.InvokeProcess();

    // send a release for each tap
    glm::vec2 pos;
    while (!this->touchTaps.Empty()) {
        this->touchTaps.Dequeue(pos);
        this->rootWidget.InvokePointerUp((int)pos.x, (int)pos.y, this->modifierKeys, true);
    }
}

//-----------------------------------------------------------------------------
void
tbMgr::Draw() {
    o_assert_dbg(this->IsValid());
    o_assert_dbg(this->renderer);

    const DisplayAttrs& disp = Gfx::RenderTargetAttrs();
    this->rootWidget.SetRect(TBRect(0, 0, disp.FramebufferWidth, disp.FramebufferHeight));

    TBAnimationManager::Update();

    this->renderer->BeginPaint(disp.FramebufferWidth, disp.FramebufferHeight);
    this->rootWidget.InvokePaint(TBWidget::PaintProps());
    this->renderer->EndPaint();

    // if animations are running, reinvalidate immediately
    if (TBAnimationManager::HasAnimationsRunning()) {
        this->rootWidget.Invalidate();
    }
}

//-----------------------------------------------------------------------------
tbOryolRootWidget*
tbMgr::GetRootWidget() {
    o_assert_dbg(this->IsValid());
    return &this->rootWidget;
}

//-----------------------------------------------------------------------------
void
tbMgr::onMouseMove(int posX, int posY) {
    this->mouseX = posX;
    this->mouseY = posY;
    this->rootWidget.InvokePointerMove(this->mouseX, this->mouseY, this->modifierKeys, false);
}

//-----------------------------------------------------------------------------
void
tbMgr::onMouseButton(Mouse::Button btn, bool down) {
    if (btn == Mouse::LMB) {
        if (down) {
            this->rootWidget.InvokePointerDown(this->mouseX, this->mouseY, 1, this->modifierKeys, false);
        }
        else {
            this->rootWidget.InvokePointerUp(this->mouseX, this->mouseY, this->modifierKeys, false);
        }
    }
    else if (btn == Mouse::RMB) {
        this->rootWidget.InvokePointerMove(this->mouseX, this->mouseY, this->modifierKeys, false);
        if (TBWidget::hovered_widget)
        {
            TBWidget::hovered_widget->ConvertFromRoot(this->mouseX, this->mouseY);
            TBWidgetEvent ev(EVENT_TYPE_CONTEXT_MENU, this->mouseX, this->mouseY, false, this->modifierKeys);
            TBWidget::hovered_widget->InvokeEvent(ev);
        }
    }
}

//-----------------------------------------------------------------------------
void
tbMgr::onTapped(const glm::vec2& pos) {
    this->touchTaps.Enqueue(pos);
    this->rootWidget.InvokePointerDown((int)pos.x, (int)pos.y, 1, this->modifierKeys, true);
}

//-----------------------------------------------------------------------------
void
tbMgr::onPanningStarted(const glm::vec2& pos) {
    this->rootWidget.InvokePointerDown((int)pos.x, (int)pos.y, 1, this->modifierKeys, true);
}

//-----------------------------------------------------------------------------
void
tbMgr::onPanningEnded(const glm::vec2& pos) {
    this->rootWidget.InvokePointerUp((int)pos.x, (int)pos.y, this->modifierKeys, true);
}

//-----------------------------------------------------------------------------
void
tbMgr::onPanning(const glm::vec2& pos) {
    this->rootWidget.InvokePointerMove((int)pos.x, (int)pos.y, this->modifierKeys, true);
}

//-----------------------------------------------------------------------------
void
tbMgr::onScroll(int wheelX, int wheelY) {
    this->rootWidget.InvokeWheel(this->mouseX, this->mouseY, wheelX, wheelY, this->modifierKeys);
}

//-----------------------------------------------------------------------------
static int
toupr_ascii(int ascii)
{
    if (ascii >= 'a' && ascii <= 'z') {
        return ascii + 'A' - 'a';
    }
    return ascii;
}

//-----------------------------------------------------------------------------
static bool
invokeShortcut(int key, SPECIAL_KEY specialKey, MODIFIER_KEYS modifierKeys, bool down) {

    #ifdef FIPS_OSX
    bool shortcut_key = (modifierKeys & TB_SUPER) ? true : false;
    #else
    bool shortcut_key = (modifierKeys & TB_CTRL) ? true : false;
    #endif
    if (!TBWidget::focused_widget || !down || !shortcut_key) {
        return false;
    }
    bool reverse_key = (modifierKeys & TB_SHIFT) ? true : false;
    int upper_key = toupr_ascii(key);
    TBID id;
    if (upper_key == 'X') {
        id = TBIDC("cut");
    }
    else if (upper_key == 'C' || specialKey == TB_KEY_INSERT) {
        id = TBIDC("copy");
    }
    else if (upper_key == 'V' || (specialKey == TB_KEY_INSERT && reverse_key)) {
        id = TBIDC("paste");
    }
    else if (upper_key == 'A') {
        id = TBIDC("selectall");
    }
    else if (upper_key == 'Z' || upper_key == 'Y') {
        bool undo = upper_key == 'Z';
        if (reverse_key) {
            undo = !undo;
        }
        id = undo ? TBIDC("undo") : TBIDC("redo");
    }
    else if (upper_key == 'N') {
        id = TBIDC("new");
    }
    else if (upper_key == 'O') {
        id = TBIDC("open");
    }
    else if (upper_key == 'S') {
        id = TBIDC("save");
    }
    else if (upper_key == 'W') {
        id = TBIDC("close");
    }
    else if (specialKey == TB_KEY_PAGE_UP) {
        id = TBIDC("prev_doc");
    }
    else if (specialKey == TB_KEY_PAGE_DOWN) {
        id = TBIDC("next_doc");
    }
    else {
        return false;
    }

    TBWidgetEvent ev(EVENT_TYPE_SHORTCUT);
    ev.modifierkeys = modifierKeys;
    ev.ref_id = id;
    return TBWidget::focused_widget->InvokeEvent(ev);
}

//-----------------------------------------------------------------------------
static bool
invokeKey(tbOryolRootWidget* rootWidget, unsigned int key, SPECIAL_KEY specialKey, MODIFIER_KEYS modifierKeys, bool down)
{
    if (invokeShortcut(key, specialKey, modifierKeys, down)) {
        return true;
    }
    rootWidget->InvokeKey(key, specialKey, modifierKeys, down);
    return true;
}

//-----------------------------------------------------------------------------
void
tbMgr::onWChar(wchar_t c) {
    if ((c >= 0xE000) && (c <= 0xF8FF)) {
        return;
    }
    invokeKey(&this->rootWidget, c, TB_KEY_UNDEFINED, this->modifierKeys, true);
    invokeKey(&this->rootWidget, c, TB_KEY_UNDEFINED, this->modifierKeys, false);
}

//-----------------------------------------------------------------------------
void
tbMgr::onKey(Key::Code key, bool down, bool up) {

    SPECIAL_KEY tbKey = TB_KEY_UNDEFINED;
    switch (key)
    {
        case Key::F1:       tbKey = TB_KEY_F1; break;
        case Key::F2:       tbKey = TB_KEY_F2; break;
        case Key::F3:       tbKey = TB_KEY_F3; break;
        case Key::F4:       tbKey = TB_KEY_F4; break;
        case Key::F5:       tbKey = TB_KEY_F5; break;
        case Key::F6:       tbKey = TB_KEY_F6; break;
        case Key::F7:       tbKey = TB_KEY_F7; break;
        case Key::F8:       tbKey = TB_KEY_F8; break;
        case Key::F9:       tbKey = TB_KEY_F9; break;
        case Key::F10:      tbKey = TB_KEY_F10; break;
        case Key::F11:      tbKey = TB_KEY_F11; break;
        case Key::F12:      tbKey = TB_KEY_F12; break;
        case Key::Left:     tbKey = TB_KEY_LEFT; break;
        case Key::Up:       tbKey = TB_KEY_UP; break;
        case Key::Right:    tbKey = TB_KEY_RIGHT; break;
        case Key::Down:     tbKey = TB_KEY_DOWN; break;
        case Key::PageUp:   tbKey = TB_KEY_PAGE_UP; break;
        case Key::PageDown: tbKey = TB_KEY_PAGE_DOWN; break;
        case Key::Home:     tbKey = TB_KEY_HOME; break;
        case Key::End:      tbKey = TB_KEY_END; break;
        case Key::Insert:   tbKey = TB_KEY_INSERT; break;
        case Key::Tab:      tbKey = TB_KEY_TAB; break;
        case Key::Delete:   tbKey = TB_KEY_DELETE; break;
        case Key::BackSpace:    tbKey = TB_KEY_BACKSPACE; break;
        case Key::Enter:
        case Key::NumEnter:
            tbKey = TB_KEY_ENTER; break;
        case Key::Escape:   tbKey = TB_KEY_ESC; break;
        case Key::Menu:
            if (TBWidget::focused_widget && !down) {
                TBWidgetEvent ev(EVENT_TYPE_CONTEXT_MENU);
                ev.modifierkeys = modifierKeys;
                TBWidget::focused_widget->InvokeEvent(ev);
            }
            break;
        case Key::LeftShift:
        case Key::RightShift:
            if (down) {
                this->modifierKeys |= TB_SHIFT;
            }
            else {
                this->modifierKeys &= ~TB_SHIFT;
            }
            break;
        case Key::LeftControl:
        case Key::RightControl:
            if (down) {
                this->modifierKeys |= TB_CTRL;
            }
            else {
                this->modifierKeys &= ~TB_CTRL;
            }
            break;
        case Key::LeftAlt:
        case Key::RightAlt:
            if (down) {
                this->modifierKeys |= TB_ALT;
            }
            else {
                this->modifierKeys &= ~TB_ALT;
            }
            break;
        case Key::LeftSuper:
        case Key::RightSuper:
            if (down) {
                this->modifierKeys |= TB_SUPER;
            }
            else {
                this->modifierKeys &= ~TB_SUPER;
            }
            break;
        default:
            break;
    }
    if (TB_KEY_UNDEFINED != tbKey) {
        invokeKey(&this->rootWidget, 0, tbKey, this->modifierKeys, down);
    }
}

} // namespace _priv
} // namespace Oryol


