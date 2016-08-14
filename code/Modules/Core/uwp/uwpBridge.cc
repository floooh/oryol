//------------------------------------------------------------------------------
//  uwpBridge.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "uwpBridge.h"
#include "Core/Assertion.h"
#include "Core/App.h"
#include <ppltasks.h>

namespace Oryol {
namespace _priv {

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::Graphics::Display;

ref class uwpApp sealed : public Windows::ApplicationModel::Core::IFrameworkView {
public:
    // called when IFrameworkView is being created
    virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ appView) {
        // register app lifecycle event handlers
        appView->Activated += 
            ref new TypedEventHandler<CoreApplicationView^,IActivatedEventArgs^>(this, &uwpApp::OnActivated);
        CoreApplication::Suspending +=
            ref new EventHandler<SuspendingEventArgs^>(this, &uwpApp::OnSuspending);
        CoreApplication::Resuming +=
            ref new EventHandler<Platform::Object^>(this, &uwpApp::OnResuming);
    }

    /// called when CoreWindow object is create (or re-created)
    virtual void SetWindow(Windows::UI::Core::CoreWindow^ window) {
        // register window event handlers
        window->SizeChanged +=
            ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &uwpApp::OnWindowSizeChanged);
        window->VisibilityChanged +=
            ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &uwpApp::OnVisibilityChanged);
        window->Closed +=
            ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &uwpApp::OnWindowClosed);

        // register display event handlers
        DisplayInformation^ disp = DisplayInformation::GetForCurrentView();
        disp->DpiChanged +=
            ref new TypedEventHandler<DisplayInformation^, Object^>(this, &uwpApp::OnDpiChanged);
        disp->OrientationChanged +=
            ref new TypedEventHandler<DisplayInformation^, Object^>(this, &uwpApp::OnOrientationChanged);
        DisplayInformation::DisplayContentsInvalidated +=
            ref new TypedEventHandler<DisplayInformation^, Object^>(this, &uwpApp::OnDisplayContentsInvalidated);        
    }

    // load previously saved app state (FIXME: wtf do I do here...)
    virtual void Load(Platform::String^ entryPoint) {
        Log::Info("uwpApp::Load() called!");  
    }
    virtual void Run() {
        while (!this->windowClosed) {
            if (this->windowVisible) {
                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
                uwpBridge::ptr()->onFrame();
            }
            else {
                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
            }
        }        
    }
    virtual void Uninitialize() { };

    // Application lifecycle event handlers.
    void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ appView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args) {
        // Run() won't start until the CoreWindow is activated
        CoreWindow::GetForCurrentThread()->Activate();
    }
    void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args) {
        // Save app state asynchronously after requesting a deferral. Holding a deferral
        // indicates that the application is busy performing suspending operations. Be
        // aware that a deferral may not be held indefinitely. After about five seconds,
        // the app will be forced to exit.
        SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

        create_task([this, deferral]()
        {
            // FIXME: Insert your code here.
            deferral->Complete();
        });
    }
    void OnResuming(Platform::Object^ sender, Platform::Object^ args) {
        // Restore any data or state that was unloaded on suspend. By default, data
        // and state are persisted when resuming from suspend. Note that this event
        // does not occur if the app was previously terminated.

        // Insert your code here.
    }

    // Window event handlers.
    void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args) {
        Log::Info("uwpApp::OnWindowSizeChanged\n");
    }
    void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args) {
        Log::Info("uwpApp::OnVisibilityChanged\n");
    }
    void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args) {
        Log::Info("uwpApp::OnWindowClosed\n");
    }

    // DisplayInformation event handlers.
    void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args) {
        Log::Info("uwpApp::OnDpiChanged\n");
    }
    void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args) {
        Log::Info("OnOrientationChanged\n");
    }
    void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args) {
        Log::Info("OnDisplayContentsInvalidate\n");
    }

private:
    bool windowClosed = false;
    bool windowVisible = true;
};

ref class uwpAppSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource {
public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView() {
        return ref new uwpApp();
    }
};

uwpBridge* uwpBridge::self = nullptr;

//------------------------------------------------------------------------------
uwpBridge::uwpBridge() :
app(nullptr) {
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
uwpBridge::~uwpBridge() {
    o_assert(nullptr == this->app);
    o_assert(nullptr != self);
    self = nullptr;
}

//------------------------------------------------------------------------------
uwpBridge*
uwpBridge::ptr() {
    o_assert_dbg(nullptr != self);
    return self;
}

//------------------------------------------------------------------------------
void
uwpBridge::setup(App* app_) {
    o_assert(nullptr == this->app);
    o_assert(nullptr != app_);
    this->app = app_;
    Log::Info("uwpBridge::setup() called!\n");
}

//------------------------------------------------------------------------------
void
uwpBridge::discard() {
    o_assert(nullptr != this->app);
    Log::Info("uwpBridge::discard() called!\n");
    this->app = nullptr;
}

//------------------------------------------------------------------------------
void
uwpBridge::onFrame() {
    o_assert_dbg(nullptr != this->app);
    this->app->onFrame();
}

//------------------------------------------------------------------------------
void
uwpBridge::startMainLoop() {
    Log::Info("uwpBridge::startMainLoop() called!\n");
    auto app = ref new uwpAppSource();
    Windows::ApplicationModel::Core::CoreApplication::Run(app);
}

} // namespace _priv
} // namespace Oryol
