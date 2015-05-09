#ifndef LISTWINDOW_DEMO_H
#define LISTWINDOW_DEMO_H

#include "Demo.h"
#include "tb_select.h"

class AdvancedItemSource;

/** Shows a list of items from *any* type of source. */
class ListWindow : public DemoWindow
{
public:
	ListWindow(TBSelectItemSource *source);
	virtual bool OnEvent(const TBWidgetEvent &ev);
};

/** Shows a list of items from a source of type AdvancedItemSource. */
class AdvancedListWindow : public DemoWindow
{
public:
	AdvancedListWindow(AdvancedItemSource *source);
	virtual bool OnEvent(const TBWidgetEvent &ev);
private:
	AdvancedItemSource *m_source;
};

/** AdvancedItem adds extra info to a string item. */
class AdvancedItem : public TBGenericStringItem
{
public:
	AdvancedItem(const char *str, const TBID &id, bool male)
		: TBGenericStringItem(str, id)
		, m_checked(false)
		, m_male(male) {}
	void SetChecked(bool checked) { m_checked = checked; }
	bool GetChecked() const { return m_checked; }
	bool GetMale() const { return m_male; }
private:
	TBStr m_info;
	bool m_checked;
	bool m_male;
};

/** AdvancedItemSource provides items of type AdvancedItem and makes sure
	the viewer is populated with the customized widget for each item. */
class AdvancedItemSource : public TBSelectItemSourceList<AdvancedItem>
{
public:
	virtual bool Filter(int index, const char *filter);
	virtual TBWidget *CreateItemWidget(int index, TBSelectItemViewer *viewer);
};

/** AdvancedItemWidget is the widget representing a AdvancedItem.
	On changes to the item, it calls InvokeItemChanged on the source, so that all
	viewers of the source are updated to reflect the change. */
class AdvancedItemWidget : public TBLayout
{
public:
	AdvancedItemWidget(AdvancedItem *item, AdvancedItemSource *source, TBSelectItemViewer *source_viewer, int index);
	virtual bool OnEvent(const TBWidgetEvent &ev);
private:
	AdvancedItemSource *m_source;
	TBSelectItemViewer *m_source_viewer;
	int m_index;
};

#endif // LISTWINDOW_DEMO_H
