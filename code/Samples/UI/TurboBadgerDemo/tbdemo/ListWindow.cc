#include "ListWindow.h"

// == AdvancedItemWidget ======================================================

AdvancedItemWidget::AdvancedItemWidget(AdvancedItem *item, AdvancedItemSource *source,
										TBSelectItemViewer *source_viewer, int index)
	: m_source(source)
	, m_source_viewer(source_viewer)
	, m_index(index)
{
	SetSkinBg(TBIDC("TBSelectItem"));
	SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
	SetLayoutDistributionPosition(LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP);
	SetPaintOverflowFadeout(false);

	g_widgets_reader->LoadFile(GetContentRoot(), "Demo/demo01/ui_resources/test_list_item.tb.txt");
	TBCheckBox *checkbox = GetWidgetByIDAndType<TBCheckBox>(TBIDC("check"));
	TBTextField *name = GetWidgetByIDAndType<TBTextField>(TBIDC("name"));
	TBTextField *info = GetWidgetByIDAndType<TBTextField>(TBIDC("info"));
	checkbox->SetValue(item->GetChecked() ? true : false);
	name->SetText(item->str);
	info->SetText(item->GetMale() ? "Male" : "Female");
}

bool AdvancedItemWidget::OnEvent(const TBWidgetEvent &ev)
{
	if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("check"))
	{
		AdvancedItem *item = m_source->GetItem(m_index);
		item->SetChecked(ev.target->GetValue() ? true : false);

		m_source->InvokeItemChanged(m_index, m_source_viewer);
		return true;
	}
	else if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("delete"))
	{
		m_source->DeleteItem(m_index);
		return true;
	}
	return TBLayout::OnEvent(ev);
}

// == AdvancedItemSource ======================================================

bool AdvancedItemSource::Filter(int index, const char *filter)
{
	// Override this method so we can return hits for our extra data too.

	if (TBSelectItemSource::Filter(index, filter))
		return true;

	AdvancedItem *item = GetItem(index);
	return stristr(item->GetMale() ? "Male" : "Female", filter) ? true : false;
}

TBWidget *AdvancedItemSource::CreateItemWidget(int index, TBSelectItemViewer *viewer)
{
	if (TBLayout *layout = new AdvancedItemWidget(GetItem(index), this, viewer, index))
		return layout;
	return nullptr;
}

// == ListWindow ==============================================================

ListWindow::ListWindow(TBSelectItemSource *source)
{
	LoadResourceFile("Demo/demo01/ui_resources/test_select.tb.txt");
	if (TBSelectList *select = GetWidgetByIDAndType<TBSelectList>("list"))
	{
		select->SetSource(source);
		select->GetScrollContainer()->SetScrollMode(SCROLL_MODE_Y_AUTO);
	}
}

bool ListWindow::OnEvent(const TBWidgetEvent &ev)
{
	if (ev.type == EVENT_TYPE_CHANGED && ev.target->GetID() == TBIDC("filter"))
	{
		if (TBSelectList *select = GetWidgetByIDAndType<TBSelectList>("list"))
			select->SetFilter(ev.target->GetText());
		return true;
	}
	return DemoWindow::OnEvent(ev);
}

// == AdvancedListWindow ==============================================================

AdvancedListWindow::AdvancedListWindow(AdvancedItemSource *source)
	: m_source(source)
{
	LoadResourceFile("Demo/demo01/ui_resources/test_select_advanced.tb.txt");
	if (TBSelectList *select = GetWidgetByIDAndType<TBSelectList>("list"))
	{
		select->SetSource(source);
		select->GetScrollContainer()->SetScrollMode(SCROLL_MODE_X_AUTO_Y_AUTO);
	}
}

bool AdvancedListWindow::OnEvent(const TBWidgetEvent &ev)
{
	TBSelectList *select = GetWidgetByIDAndType<TBSelectList>("list");
	if (select && ev.type == EVENT_TYPE_CHANGED && ev.target->GetID() == TBIDC("filter"))
	{
		select->SetFilter(ev.target->GetText());
		return true;
	}
	else if (select && ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("add"))
	{
		TBStr name = GetTextByID(TBIDC("add_name"));
		if (!name.IsEmpty())
			m_source->AddItem(new AdvancedItem(name, TBIDC("boy_item"), true));
		return true;
	}
	else if (select && ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == TBIDC("delete all"))
	{
		m_source->DeleteAllItems();
		return true;
	}
	return DemoWindow::OnEvent(ev);
}
