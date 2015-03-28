#include "ResourceEditWindow.h"
#include "tb_widgets_reader.h"
#include "tb_message_window.h"
#include "tb_system.h"
#include "tb_select.h"
#include "tb_editfield.h"
#include "tb_tempbuffer.h"
#include "tb_scroll_container.h"
#include <stdio.h>

// == ResourceItem ====================================================================================

ResourceItem::ResourceItem(TBWidget *widget, const char *str)
	: TBGenericStringItem(str)
	, m_widget(widget)
{
}

// == ResourceEditWindow ==============================================================================

ResourceEditWindow::ResourceEditWindow()
	: m_widget_list(nullptr)
	, m_scroll_container(nullptr)
	, m_build_container(nullptr)
	, m_source_edit(nullptr)
{
	// Register as global listener to intercept events in the build container
	TBWidgetListener::AddGlobalListener(this);

	g_widgets_reader->LoadFile(this, "Demo/demo01/ui_resources/resource_edit_window.tb.txt");

	m_scroll_container = GetWidgetByIDAndType<TBScrollContainer>(TBIDC("scroll_container"));
	m_build_container = m_scroll_container->GetContentRoot();
	m_source_edit = GetWidgetByIDAndType<TBEditField>(TBIDC("source_edit"));

	m_widget_list = GetWidgetByIDAndType<TBSelectList>(TBIDC("widget_list"));
	m_widget_list->SetSource(&m_widget_list_source);

	SetRect(TBRect(100, 50, 900, 600));
}

ResourceEditWindow::~ResourceEditWindow()
{
	TBWidgetListener::RemoveGlobalListener(this);

	// avoid assert
	m_widget_list->SetSource(nullptr);
}

void ResourceEditWindow::Load(const char *resource_file)
{
	m_resource_filename.Set(resource_file);
	SetText(resource_file);

	// Set the text of the source view
	m_source_edit->SetText("");

	if (TBFile *file = TBFile::Open(m_resource_filename, TBFile::MODE_READ))
	{
		TBTempBuffer buffer;
		if (buffer.Reserve(file->Size()))
		{
			uint32 size_read = file->Read(buffer.GetData(), 1, buffer.GetCapacity());
			m_source_edit->SetText(buffer.GetData(), size_read);
		}
		delete file;
	}
	else // Error, show message
	{
		TBStr text;
		text.SetFormatted("Could not load file %s", resource_file);
		if (TBMessageWindow *msg_win = new TBMessageWindow(GetParentRoot(), TBIDC("")))
			msg_win->Show("Error loading resource", text);
	}

	RefreshFromSource();
}

void ResourceEditWindow::RefreshFromSource()
{
	// Clear old widgets
	while (TBWidget *child = m_build_container->GetFirstChild())
	{
		m_build_container->RemoveChild(child);
		delete child;
	}

	// Create new widgets from source
	g_widgets_reader->LoadData(m_build_container, m_source_edit->GetText());

	// Force focus back in case the edited resource has autofocus.
	// FIX: It would be better to prevent the focus change instead!
	m_source_edit->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);
}

void ResourceEditWindow::UpdateWidgetList(bool immediately)
{
	if (!immediately)
	{
		TBID id = TBIDC("update_widget_list");
		if (!GetMessageByID(id))
			PostMessage(id, nullptr);
	}
	else
	{
		m_widget_list_source.DeleteAllItems();
		AddWidgetListItemsRecursive(m_build_container, 0);

		m_widget_list->InvalidateList();
	}
}

void ResourceEditWindow::AddWidgetListItemsRecursive(TBWidget *widget, int depth)
{
	if (depth > 0) // Ignore the root
	{
		// Add a new ResourceItem for this widget
		TBStr str;
		const char *classname = widget->GetClassName();
		if (!*classname)
			classname = "<Unknown widget type>";
		str.SetFormatted("% *s%s", depth - 1, "", classname);

		if (ResourceItem *item = new ResourceItem(widget, str))
			m_widget_list_source.AddItem(item);
	}

	for (TBWidget *child = widget->GetFirstChild(); child; child = child->GetNext())
		AddWidgetListItemsRecursive(child, depth + 1);
}

ResourceEditWindow::ITEM_INFO ResourceEditWindow::GetItemFromWidget(TBWidget *widget)
{
	ITEM_INFO item_info = { nullptr, -1 };
	for (int i = 0; i < m_widget_list_source.GetNumItems(); i++)
		if (m_widget_list_source.GetItem(i)->GetWidget() == widget)
		{
			item_info.index = i;
			item_info.item = m_widget_list_source.GetItem(i);
			break;
		}
	return item_info;
}

void ResourceEditWindow::SetSelectedWidget(TBWidget *widget)
{
	m_selected_widget.Set(widget);
	ITEM_INFO item_info = GetItemFromWidget(widget);
	if (item_info.item)
		m_widget_list->SetValue(item_info.index);
}

bool ResourceEditWindow::OnEvent(const TBWidgetEvent &ev)
{
	if (ev.type == EVENT_TYPE_CHANGED && ev.target->GetID() == TBIDC("widget_list_search"))
	{
		m_widget_list->SetFilter(ev.target->GetText());
		return true;
	}
	else if (ev.type == EVENT_TYPE_CHANGED && ev.target == m_widget_list)
	{
		if (m_widget_list->GetValue() >= 0 && m_widget_list->GetValue() < m_widget_list_source.GetNumItems())
			if (ResourceItem *item = m_widget_list_source.GetItem(m_widget_list->GetValue()))
				SetSelectedWidget(item->GetWidget());
	}
	else if (ev.type == EVENT_TYPE_CHANGED && ev.target == m_source_edit)
	{
		RefreshFromSource();
		return true;
	}
	else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("test"))
	{
		// Create a window containing the current layout, resize and center it.
		if (TBWindow *win = new TBWindow())
		{
			win->SetText("Test window");
			g_widgets_reader->LoadData(win->GetContentRoot(), m_source_edit->GetText());
			TBRect bounds(0, 0, GetParent()->GetRect().w, GetParent()->GetRect().h);
			win->SetRect(win->GetResizeToFitContentRect().CenterIn(bounds).MoveIn(bounds).Clip(bounds));
			GetParent()->AddChild(win);
		}
		return true;
	}
	else if (ev.target->GetID() == TBIDC("constrained"))
	{
		m_scroll_container->SetAdaptContentSize(ev.target->GetValue() ? true : false);
		return true;
	}
	else if (ev.type == EVENT_TYPE_FILE_DROP)
	{
		return OnDropFileEvent(ev);
	}
	return TBWindow::OnEvent(ev);
}

void ResourceEditWindow::OnPaintChildren(const PaintProps &paint_props)
{
	TBWindow::OnPaintChildren(paint_props);

	// Paint the selection of the selected widget
	if (TBWidget *selected_widget = GetSelectedWidget())
	{
		TBRect widget_rect(0, 0, selected_widget->GetRect().w, selected_widget->GetRect().h);
		selected_widget->ConvertToRoot(widget_rect.x, widget_rect.y);
		ConvertFromRoot(widget_rect.x, widget_rect.y);
		g_renderer->DrawRect(widget_rect, TBColor(255, 205, 0));
	}
}

void ResourceEditWindow::OnMessageReceived(TBMessage *msg)
{
	if (msg->message == TBIDC("update_widget_list"))
		UpdateWidgetList(true);
}

bool ResourceEditWindow::OnWidgetInvokeEvent(TBWidget *widget, const TBWidgetEvent &ev)
{
	// Intercept all events to widgets in the build container
	if (m_build_container->IsAncestorOf(ev.target))
	{
		// Let events through if alt is pressed so we can test some
		// functionality right in the editor (like toggle hidden UI).
		if (ev.modifierkeys & TB_ALT)
			return false;

		// Select widget when clicking
		if (ev.type == EVENT_TYPE_POINTER_DOWN)
			SetSelectedWidget(ev.target);

		if (ev.type == EVENT_TYPE_FILE_DROP)
			OnDropFileEvent(ev);
		return true;
	}
	return false;
}

void ResourceEditWindow::OnWidgetAdded(TBWidget *parent, TBWidget *child)
{
	if (m_build_container && m_build_container->IsAncestorOf(child))
		UpdateWidgetList(false);
}

void ResourceEditWindow::OnWidgetRemove(TBWidget *parent, TBWidget *child)
{
	if (m_build_container && m_build_container->IsAncestorOf(child))
		UpdateWidgetList(false);
}

bool ResourceEditWindow::OnDropFileEvent(const TBWidgetEvent &ev)
{
	const TBWidgetEventFileDrop *fd_event = TBSafeCast<TBWidgetEventFileDrop>(&ev);
	if (fd_event->files.GetNumItems() > 0)
		Load(*fd_event->files.Get(0));
	return true;
}
