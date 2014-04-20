//------------------------------------------------------------------------------
//  CoreHello.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/AppBase.h"
#include "Core/Log.h"

OryolApp("CoreHello", "1.0");

using namespace Oryol;
using namespace Oryol::Core;

class CoreHelloApp : public AppBase {
public:
    virtual void onFrame();
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
    Log::Info("Hello World!\n");
    this->stopMainLoop();
    AppBase::onFrame();
}
