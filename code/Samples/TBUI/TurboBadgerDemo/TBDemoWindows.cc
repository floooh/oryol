//------------------------------------------------------------------------------
//  TBDemoWindows.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TBDemoWindows.h"
#include "tb_message_window.h"
#include "animation/tb_widget_animation.h"
#include "tb_select.h"
#include "tb_inline_select.h"
#include "tb_system.h"
#include "TBUI/TBUI.h"

using namespace Oryol;
using namespace tb;

//------------------------------------------------------------------------------
MainWindow::MainWindow() {
    this->LoadResourceFile(GetMainResource());
    this->SetOpacity(0.97f);
}

//------------------------------------------------------------------------------
URL
MainWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_ui.tb.txt";
}

//------------------------------------------------------------------------------
void
MainWindow::OnMessageReceived(TBMessage* msg) {
    Log::Info("MainWindow::OnMessageReceived()\n");
}

//------------------------------------------------------------------------------
bool
MainWindow::OnEvent(const TBWidgetEvent& ev) {
    if (ev.type == EVENT_TYPE_CLICK) {
        if (ev.target->GetID() == TBIDC("new")) {
            new MainWindow();
            return true;
        }
        if (ev.target->GetID() == TBIDC("msg")) {
            this->PostMessage(TBIDC("instantmsg"), nullptr);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("busymsg")) {
            if (ev.target->GetValue() == 1) {
                // Post the first "busy" message when we check the checkbox.
                o_assert_dbg(!this->GetMessageByID(TBIDC("busy")));
                if (!this->GetMessageByID(TBIDC("busy"))) {
                    this->PostMessage(TBIDC("busy"), nullptr);
                    TBMessageWindow *msg_win = new TBMessageWindow(this, TBIDC("test_dialog"));
                    msg_win->Show("Message window", "The message loop is now constantly busy with messages to process.\n\n"
                        "The main thread should be working hard, but input & animations should still be running smoothly.");
                }
            }
            else {
                // Remove any pending "busy" message when we uncheck the checkbox.
                o_assert_dbg(this->GetMessageByID(TBIDC("busy")));
                if (TBMessage *busymsg = this->GetMessageByID(TBIDC("busy"))) {
                    this->DeleteMessage(busymsg);
                }
            }
			return true;
		}
        else if (ev.target->GetID() == TBIDC("delayedmsg")) {
            this->PostMessageDelayed(TBIDC("delayedmsg"), nullptr, 2000);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("TBWindow.close")) {
            // Intercept the TBWindow.close message and stop it from bubbling
            // to TBWindow (prevent the window from closing)
            TBMessageWindow *msg_win = new TBMessageWindow(this, TBIDC("confirm_close_dialog"));
            TBMessageWindowSettings settings(TB_MSG_YES_NO, TBIDC("Icon48"));
            settings.dimmer = true;
            settings.styling = true;
            msg_win->Show("Are you sure?", "Really <color #0794f8>close</color> the window?", &settings);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("confirm_close_dialog")) {
            if (ev.ref_id == TBIDC("TBMessageWindow.yes")) {
                this->Close();
            }
            return true;
        }
        else if (ev.target->GetID() == TBIDC("reload skin bitmaps")) {
            int reload_count = 10;
            double t1 = TBSystem::GetTimeMS();
            for (int i = 0; i < reload_count; i++) {
                g_tb_skin->ReloadBitmaps();
            }
            double t2 = TBSystem::GetTimeMS();

            TBStr message;
            message.SetFormatted("Reloading the skin graphics %d times took %dms", reload_count, (int)(t2 - t1));
            TBMessageWindow *msg_win = new TBMessageWindow(ev.target, TBID());
            msg_win->Show("GFX load performance", message);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test context lost")) {
            g_renderer->InvokeContextLost();
            g_renderer->InvokeContextRestored();
            TBMessageWindow *msg_win = new TBMessageWindow(ev.target, TBID());
            msg_win->Show("Context lost & restore",
                "Called InvokeContextLost and InvokeContextRestored.\n\n"
                "Does everything look fine?");
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-layout")) {
        /*
            TBStr resource_file("Demo/demo01/ui_resources/");
            resource_file.Append(ev.target->data.GetString());
            new LayoutWindow(resource_file);
        */
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-connections")) {
            //new ConnectionWindow();
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-list")) {
            //new AdvancedListWindow(&advanced_source);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-image")) {
            TBUI::DoAfter(Array<URL>({
                ImageWindow::GetMainResource(),
                "ui:demo/images/image_1.png",
                "ui:demo/images/image_2.png",
                "ui:demo/images/image_3.png",
                "ui:demo/images/image_4.png",
                "ui:demo/images/image_5.png",
                "ui:demo/images/image_6.png",
                "ui:demo/images/image_7.png",
                "ui:demo/images/image_8.png",
                "ui:demo/images/image_9.png"
            }), [] {
                new ImageWindow();
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-page")) {
            TBUI::DoAfter(PageWindow::GetMainResource(), [] {
                new PageWindow();
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-animations")) {
            TBUI::DoAfter(AnimationsWindow::GetMainResource(), [] {
                new AnimationsWindow();
            });
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-scroll-container")) {
            //new ScrollContainerWindow();
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-skin-conditions")) {
            /*
            (new DemoWindow())->LoadResourceFile("Demo/demo01/ui_resources/test_skin_conditions01.tb.txt");
            (new DemoWindow())->LoadResourceFile("Demo/demo01/ui_resources/test_skin_conditions02.tb.txt");
            */
            return true;
        }
        else if (ev.target->GetID() == TBIDC("test-resource-edit")) {
            /*
            ResourceEditWindow *res_edit_win = new ResourceEditWindow();
            res_edit_win->Load("Demo/demo01/ui_resources/resource_edit_test.tb.txt");
            this->GetParent()->AddChild(res_edit_win);
            */
            return true;
        }
        else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("debug settings")) {
            #ifdef TB_RUNTIME_DEBUG_INFO
            this->ShowDebugInfoSettingsWindow(GetParentRoot());
            #else
            TBMessageWindow *msg_win = new TBMessageWindow(ev.target, TBID());
            msg_win->Show("Debug settings",
                "Debug settings is only available in builds "
                "compiled with TB_RUNTIME_DEBUG_INFO defined.\n\n"
                "Debug builds enable this by default.");
            #endif
            return true;
        }
    }
    return TBUIWindow::OnEvent(ev);
}

//------------------------------------------------------------------------------
ImageWindow::ImageWindow() {
    this->LoadResourceFile(GetMainResource());
}

//------------------------------------------------------------------------------
URL
ImageWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_image_widget.tb.txt";
}

//------------------------------------------------------------------------------
bool ImageWindow::OnEvent(const TBWidgetEvent &ev) {
    if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("remove")) {
        TBWidget *image = ev.target->GetParent();
        image->GetParent()->RemoveChild(image);
        delete image;
        return true;
    }
    return TBUIWindow::OnEvent(ev);
}

//------------------------------------------------------------------------------
PageWindow::PageWindow() {
    this->LoadResourceFile(GetMainResource());

    // Listen to the pagers scroller
    if (TBWidget *pager = GetWidgetByID(TBIDC("page-scroller")))
        pager->GetScroller()->SetSnapListener(this);
}

//------------------------------------------------------------------------------
URL
PageWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_scroller_snap.tb.txt";
}

//------------------------------------------------------------------------------
void
PageWindow::OnScrollSnap(TBWidget *target_widget, int &target_x, int &target_y) {
    int page_w = target_widget->GetPaddingRect().w;
    int target_page = (target_x + page_w / 2) / page_w;
    target_x = target_page * page_w;
}

//------------------------------------------------------------------------------
AnimationsWindow::AnimationsWindow() {
    this->LoadResourceFile(GetMainResource());
    this->Animate();
}

//------------------------------------------------------------------------------
URL
AnimationsWindow::GetMainResource() {
    return "ui:demo/ui_resources/test_animations.tb.txt";
}

//------------------------------------------------------------------------------
void
AnimationsWindow::Animate() {
    // Abort any still unfinished animations.
    TBWidgetsAnimationManager::AbortAnimations(this);

    ANIMATION_CURVE curve = ANIMATION_CURVE_SLOW_DOWN;
    double duration = 500;
    bool fade = true;

    if (TBSelectList *curve_select = GetWidgetByIDAndType<TBSelectList>("curve")) {
        curve = static_cast<ANIMATION_CURVE>(curve_select->GetValue());
    }
    if (TBInlineSelect *duration_select = GetWidgetByIDAndType<TBInlineSelect>("duration")) {
        duration = duration_select->GetValueDouble();
    }
    if (TBCheckBox *fade_check = GetWidgetByIDAndType<TBCheckBox>("fade")) {
        fade = fade_check->GetValue() ? true : false;
    }

    // Start move animation
    if (TBAnimationObject *anim = new TBWidgetAnimationRect(this, GetRect().Offset(-GetRect().x - GetRect().w, 0), GetRect())) {
        TBAnimationManager::StartAnimation(anim, curve, duration);
    }
    // Start fade animation
    if (fade) {
        if (TBAnimationObject *anim = new TBWidgetAnimationOpacity(this, TB_ALMOST_ZERO_OPACITY, 1, false)) {
            TBAnimationManager::StartAnimation(anim, ANIMATION_CURVE_SLOW_DOWN, duration);
        }
    }
}

//------------------------------------------------------------------------------
bool AnimationsWindow::OnEvent(const TBWidgetEvent &ev) {
    if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("Animate!")) {
        Animate();
    }
    return TBUIWindow::OnEvent(ev);
}
