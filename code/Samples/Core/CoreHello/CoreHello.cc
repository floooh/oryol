//------------------------------------------------------------------------------
//  CoreHello.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/AppBase.h"
#include "Core/Log.h"
#include <thread>

OryolApp("CoreHello", "1.0");

using namespace Oryol;
using namespace Oryol::Core;

class CoreHelloApp : public AppBase {
public:
    virtual void onFrame();
private:
    int frameCount = 0;
};

//------------------------------------------------------------------------------
void
OryolMain() {
    CoreHelloApp app;
    app.StartMainLoop();
}

//------------------------------------------------------------------------------
void
CoreHelloApp::onFrame() {
    if (0 == (this->frameCount++ % 10)) {
        Log::Info("Hello World %d!\n", this->frameCount);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    AppBase::onFrame();
}
