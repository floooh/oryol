#ifndef DEMO_H
#define DEMO_H

#include "tb_widgets.h"
#include "tb_widgets_common.h"
#include "tb_widgets_reader.h"
#include "tb_widgets_listener.h"
#include "tb_message_window.h"
#include "tb_msg.h"
#include "tb_scroller.h"
#include "platform/Application.h"

using namespace tb;

class DemoApplication : public Application
{
public:
	DemoApplication() : Application() {}

	virtual bool Init();
	virtual void RenderFrame(int window_w, int window_h);
};

class DemoWindow : public TBWindow
{
public:
	DemoWindow();
	bool LoadResourceFile(const char *filename);
	void LoadResourceData(const char *data);
	void LoadResource(TBNode &node);

	virtual bool OnEvent(const TBWidgetEvent &ev);
};

class MainWindow : public DemoWindow, public TBMessageHandler
{
public:
	MainWindow();
	virtual bool OnEvent(const TBWidgetEvent &ev);

	// Implement TBMessageHandler
	virtual void OnMessageReceived(TBMessage *msg);
};

class ImageWindow : public DemoWindow
{
public:
	ImageWindow();
	virtual bool OnEvent(const TBWidgetEvent &ev);
};

class PageWindow : public DemoWindow, public TBScrollerSnapListener
{
public:
	PageWindow();
	virtual bool OnEvent(const TBWidgetEvent &ev);
	virtual void OnScrollSnap(TBWidget *target_widget, int &target_x, int &target_y);
};

class AnimationsWindow : public DemoWindow
{
public:
	AnimationsWindow();
	void Animate();
	virtual bool OnEvent(const TBWidgetEvent &ev);
};

class LayoutWindow : public DemoWindow
{
public:
	LayoutWindow(const char *filename);
	virtual bool OnEvent(const TBWidgetEvent &ev);
};

class TabContainerWindow : public DemoWindow
{
public:
	TabContainerWindow();
	virtual bool OnEvent(const TBWidgetEvent &ev);
};

class ConnectionWindow : public DemoWindow
{
public:
	ConnectionWindow();
	virtual bool OnEvent(const TBWidgetEvent &ev);
};

class ScrollContainerWindow : public DemoWindow, public TBMessageHandler
{
public:
	ScrollContainerWindow();
	virtual bool OnEvent(const TBWidgetEvent &ev);

	// Implement TBMessageHandler
	virtual void OnMessageReceived(TBMessage *msg);
};

#endif // DEMO_H
