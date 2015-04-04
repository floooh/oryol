//------------------------------------------------------------------------------
//  TBUIWindow.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TBUI/TBUI.h"
#include "TBUIWindow.h"
#include "tb_skin.h"
#include "tb_node_tree.h"
#include "tb_widgets_reader.h"

namespace Oryol {

OryolClassImpl(TBUIWindow);

//------------------------------------------------------------------------------
TBUIWindow::TBUIWindow() :
tbWindow(nullptr),
resLabel(ResourceLabel::Invalid) {
    // empty
}

//------------------------------------------------------------------------------
TBUIWindow::~TBUIWindow() {
    if (this->ioQueue.IsStarted()) {
        this->ioQueue.Stop();
    }
    if (this->tbWindow) {
        TBUI::getRootWidget()->RemoveChild(this->tbWindow);
        Memory::Delete(this->tbWindow);
        this->tbWindow = nullptr;
    }
}

//------------------------------------------------------------------------------
URL
TBUIWindow::MainResource() const {
    // override in subclass!
    return URL();
}

//------------------------------------------------------------------------------
Array<URL>
TBUIWindow::AdditionalResources() const {
    // optionally override in subclass
    return Array<URL>();
}

//------------------------------------------------------------------------------
void
TBUIWindow::Open() {
    Array<URL> urls = this->AdditionalResources();
    urls.Add(this->MainResource());
    this->ioQueue.Start();
    this->ioQueue.AddGroup(urls, [this](const Array<Ptr<Stream>>& streams) {
        this->resLabel = TBUI::Resource().PushLabel();
        for (const auto& stream : streams) {
            TBUI::Resource().Add(stream);
        }
        TBUI::Resource().PopLabel();
        this->OnResourcesLoaded();
    });

}

//------------------------------------------------------------------------------
void
TBUIWindow::OnResourcesLoaded() {
    o_assert_dbg(nullptr == this->tbWindow);
    this->tbWindow = Memory::New<tbWindowProxy>();
    TBUI::getRootWidget()->AddChild(this->tbWindow);
    tb::TBNode node;
    node.ReadFile(this->MainResource().AsCStr());
    this->LoadResource(node);
}

//------------------------------------------------------------------------------
void
TBUIWindow::LoadResource(tb::TBNode& node) {
    // see TurboBadger DemoWindow class
    
    tb::g_widgets_reader->LoadNodeTree(this->tbWindow, &node);

    // Get title from the WindowInfo section (or use "" if not specified)
    this->tbWindow->SetText(node.GetValueString("WindowInfo>title", ""));

    const tb::TBRect parent_rect(0, 0, this->tbWindow->GetParent()->GetRect().w, this->tbWindow->GetParent()->GetRect().h);
    const tb::TBDimensionConverter *dc = tb::g_tb_skin->GetDimensionConverter();
    tb::TBRect window_rect = this->tbWindow->GetResizeToFitContentRect();

    // Use specified size or adapt to the preferred content size.
    tb::TBNode *tmp = node.GetNode("WindowInfo>size");
    if (tmp && tmp->GetValue().GetArrayLength() == 2)
    {
        window_rect.w = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(0)->GetString(), window_rect.w);
        window_rect.h = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(1)->GetString(), window_rect.h);
    }

    // Use the specified position or center in parent.
    tmp = node.GetNode("WindowInfo>position");
    if (tmp && tmp->GetValue().GetArrayLength() == 2)
    {
        window_rect.x = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(0)->GetString(), window_rect.x);
        window_rect.y = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(1)->GetString(), window_rect.y);
    }
    else {
        window_rect = window_rect.CenterIn(parent_rect);
    }

    // Make sure the window is inside the parent, and not larger.
    window_rect = window_rect.MoveIn(parent_rect).Clip(parent_rect);
    this->tbWindow->SetRect(window_rect);

    // Ensure we have focus - now that we've filled the window with possible focusable
    // widgets. EnsureFocus was automatically called when the window was activated (by
    // adding the window to the root), but then we had nothing to focus.
    // Alternatively, we could add the window after setting it up properly.
    this->tbWindow->EnsureFocus();
}

} // namespace Oryol