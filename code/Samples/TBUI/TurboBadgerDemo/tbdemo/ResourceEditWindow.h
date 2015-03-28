#ifndef ResourceEditWindow_H
#define ResourceEditWindow_H

#include "tb_widgets.h"
#include "tb_select.h"
#include "tb_widgets_common.h"
#include "tb_widgets_listener.h"
#include "tb_editfield.h"
#include "tb_msg.h"

using namespace tb;

class ResourceItem : public TBGenericStringItem
{
public:
	ResourceItem(TBWidget *widget, const char *str);
	TBWidget *GetWidget() { return m_widget; }
private:
	TBWidget *m_widget;
};

class ResourceEditWindow : public TBWindow, public TBMessageHandler, public TBWidgetListener
{
public:
	// For safe typecasting
	TBOBJECT_SUBCLASS(ResourceEditWindow, TBWindow);

	ResourceEditWindow();
	~ResourceEditWindow();

	void UpdateWidgetList(bool immediately);

	struct ITEM_INFO {
		ResourceItem *item;
		int index;
	};
	ITEM_INFO GetItemFromWidget(TBWidget *widget);
	TBWidget *GetSelectedWidget() { return m_selected_widget.Get(); }
	void SetSelectedWidget(TBWidget *widget);

	void Load(const char *resource_file);
	void RefreshFromSource();

	// == TBWindow ======================================================================
	virtual bool OnEvent(const TBWidgetEvent &ev);
	virtual void OnPaintChildren(const PaintProps &paint_props);

	// == TBMessageHandler ==============================================================
	virtual void OnMessageReceived(TBMessage *msg);

	// == TBWidgetListener ========================================================
	virtual bool OnWidgetInvokeEvent(TBWidget *widget, const TBWidgetEvent &ev);
	virtual void OnWidgetAdded(TBWidget *parent, TBWidget *child);
	virtual void OnWidgetRemove(TBWidget *parent, TBWidget *child);
private:
	TBSelectList *m_widget_list;
	TBSelectItemSourceList<ResourceItem> m_widget_list_source;
	TBScrollContainer *m_scroll_container;
	TBWidget *m_build_container;
	TBEditField *m_source_edit;
	TBStr m_resource_filename;
	TBWidgetSafePointer m_selected_widget;
	void AddWidgetListItemsRecursive(TBWidget *widget, int depth);
	bool OnDropFileEvent(const TBWidgetEvent &ev);
};

#endif // ResourceEditWindow_H
