//-----------------------------------------------------------------------------
//  tbMgr.cc
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "tbMgr.h"
#include "Core/Core.h"
#include "IO/Core/URLBuilder.h"
#include "Gfx/Gfx.h"
#include "animation/tb_widget_animation.h"
#include "tb_font_renderer.h"

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
    this->renderer = Memory::New<tbOryolRenderer>();
    this->renderer->Setup();
	tb_core_init(this->renderer, setup.Locale.AsCStr());
    
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
    
    TBWidgetsAnimationManager::Init();

    this->isValid = true;
}

//-----------------------------------------------------------------------------
void
tbMgr::Discard() {
    o_assert_dbg(this->IsValid());
    
    TBWidgetsAnimationManager::Shutdown();
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
tbMgr::Draw() {
    o_assert_dbg(this->IsValid());
    o_assert_dbg(this->renderer);

    const DisplayAttrs& disp = Gfx::RenderTargetAttrs();
    this->rootWidget.SetRect(TBRect(0, 0, disp.FramebufferWidth, disp.FramebufferHeight));

    TBAnimationManager::Update();
    this->rootWidget.InvokeProcessStates();
    this->rootWidget.InvokeProcess();
    
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

} // namespace _priv
} // namespace Oryol


