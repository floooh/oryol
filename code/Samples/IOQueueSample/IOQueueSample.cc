//------------------------------------------------------------------------------
//  IOQueueSample.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "IO/IO.h"
#include "HttpFS/HTTPFileSystem.h"
#include <thread>
#include <chrono>

using namespace Oryol;

class IOQueueApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
};
OryolMain(IOQueueApp);

//------------------------------------------------------------------------------
AppState::Code
IOQueueApp::OnInit() {

    // NOTE: the formerly public IOQueue class is now integrated into
    // the IO facade as the functions IO::Load() and IO::LoadGroup()!

    // setup the IO module, attach a HTTP filesystem and setup
    // a path assign (aka path alias)
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("res:", ORYOL_SAMPLE_URL);
    IO::Setup(ioSetup);
    
    // now the important part: start loading files, and define
    // the success-callbacks as lambdas
    //
    // the success callback gets a smart pointer to a Stream object which
    // contains the downloaded file data
    //
    // the failure callback gets the URL and IO status code
    //
    // the failure callback optional, if none is given, a global failure
    // callback is used which terminates the app
    
    // the first 2 files only define a success callback
    IO::Load("res:lok_dxt1.dds", [](IO::LoadResult res) {
        Log::Info("File '%s' loaded!\n", res.Url.Path().AsCStr());
    });
    IO::Load("res:lok_dxt3.dds", [](IO::LoadResult res) {
        Log::Info("File '%s' loaded!\n", res.Url.Path().AsCStr());
    });

    // try to load the files as group
    IO::LoadGroup(Array<URL>({
        "res:lok_dxt1.dds",
        "res:lok_dxt3.dds",
        "res:lok_dxt5.dds"
    }), [](Array<IO::LoadResult> results) {
        for (const auto& res : results) {
            Log::Info("LoadGroup: file '%s' loaded!\n", res.Url.Path().AsCStr());
        }
    });
    
    // this tries to load a non-existing file, this should invoke the failure callback
    IO::Load("res:blablabla.xxx",
          // success callback (shouldn't be called)
          [](IO::LoadResult res) {
              Log::Info("File '%s' loaded (shouldn't happen)!\n", res.Url.Path().AsCStr());
          },
          // failure callback
          [](const URL& url, IOStatus::Code ioStatus) {
              Log::Info("Failed to load file (intended): url=%s, ioStatus=%d\n", url.Path().AsCStr(), ioStatus);
          });
    
    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
IOQueueApp::OnRunning() {

    // the IO module is now checking for finished IO requests, and will
    // call either the success or failure callback for each finished
    // request

    // sleep a bit, usually some other per-frame stuff happens here of course...
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // quit the app once the IOQueue is empty
    return IO::NumPendingLoads() > 0 ? AppState::Running : AppState::Cleanup;
}

//------------------------------------------------------------------------------
AppState::Code
IOQueueApp::OnCleanup() {
    IO::Discard();
    return AppState::Destroy;
}
