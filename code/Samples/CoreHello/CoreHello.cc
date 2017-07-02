//------------------------------------------------------------------------------
//  CoreHello.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include <thread>

using namespace Oryol;

class CoreHelloApp : public App {
public:
    virtual void onFrame() {
        if (0 == (this->frameCount++ % 10)) {
            Log::Info("Hello World %d!\n", this->frameCount);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        App::onFrame();        
    }
    int frameCount = 0;
};
OryolMain(CoreHelloApp);
