//------------------------------------------------------------------------------
//  IOQueueSample.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "IO/IOFacade.h"
#include "IO/Core/IOQueue.h"
#include "HTTP/HTTPFileSystem.h"

using namespace Oryol;

// derived application class
class IOQueueApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    IOFacade* io;
    IOQueue ioQueue;
};
OryolMain(IOQueueApp);

//------------------------------------------------------------------------------
AppState::Code
IOQueueApp::OnInit() {

    // an IOFacade is required for IOQueue to work, and we want to
    // download files from a HTTP server, so setup an HTTPFileSystem
    // define a path alias, we'll default to localhost on port 8000
    // (e.g. run 'python -m SimpleHTTPServer')
    // when running as a deployed web app all files must be loaded
    // from the own domain, the host URL part is ignored then
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("res:", "http://localhost:8000/");
    this->io = IOFacade::CreateSingle(ioSetup);
    
    // now the important part: add IO requests to the IOQueue,
    // and directly define success-callbacks as lambdas (of course
    // it is also possible to use traditional std::function objects
    // to define the callbacks
    //
    // the success callback gets a smart pointer to a Stream object which
    // contains the downloaded file data
    //
    // the failure callback gets the URL and IO status code
    //
    // the failure callback optional, if none is given, a global failure
    // callback is used which terminates the app
    
    // the first 2 files only define a success callback
    this->ioQueue.Add("res:image1.jpg", [](const Ptr<Stream>& stream) {
        Log::Info("File '%s' loaded!\n", stream->GetURL().Path().AsCStr());
    });
    this->ioQueue.Add("res:image2.jpg", [](const Ptr<Stream>& stream) {
        Log::Info("File '%s' loaded!\n", stream->GetURL().Path().AsCStr());
    });
    
    // this tries to load a non-existing file, this should invoke the failure callback
    this->ioQueue.Add("res:blablabla.xxx",
          // success callback (shouldn't be called)
          [](const Ptr<Stream>& stream) {
              Log::Info("File '%s' loaded (shouldn't happen)!\n", stream->GetURL().Path().AsCStr());
          },
          // failure callback
          [](const URL& url, IOStatus::Code ioStatus) {
              Log::Info("Failed to load file (intended): url=%s, ioStatus=%d\n", url.Path().AsCStr(), ioStatus);
          });
    
    // we need to 'start' the IOQueue, otherwise the IO requests won't
    // be processed. a started IOQueue attaches itself to the local
    // thread's runloop and will be triggered automatically once per frame,
    // the success/failure callbacks will be called from the runloop
    // from the same thread the IOQueue has been setup on
    this->ioQueue.Start();
    
    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
IOQueueApp::OnRunning() {

    // the IOQueue is now checking for finished IO requests, and will
    // call either the success or failure callback for each finished
    // request

    // sleep a bit, usually some other per-frame stuff happens here of course...
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // quit the app once the IOQueue is empty
    return this->ioQueue.Empty() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
IOQueueApp::OnCleanup() {
    
    // it's important to stop the IOQueue before the IO subsystem is destroyed,
    // usually you'd want to stop the queue as soon as it is done loading
    // all files
    this->ioQueue.Stop();
    this->io = nullptr;
    IOFacade::DestroySingle();
    
    return AppState::Destroy;
}
