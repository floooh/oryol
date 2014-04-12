//------------------------------------------------------------------------------
//  HTTPClientSample.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Application/App.h"
#include "HTTP/HTTPClient.h"
#include <thread>
#include <chrono>

OryolApp("HTTPClient", "1.0");

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;
using namespace Oryol::Application;
using namespace Oryol::HTTP;

// derived application class
class HTTPClientApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    Ptr<HTTPClient> httpClient;
    Ptr<HTTPProtocol::HTTPRequest> req;
};

//------------------------------------------------------------------------------
void
OryolMain() {
    // execution starts here, create our app and start the main loop
    HTTPClientApp app;
    app.StartMainLoop();
}

//------------------------------------------------------------------------------
AppState::Code
HTTPClientApp::OnInit() {

    this->httpClient = HTTPClient::Create();

    // fire 2 HTTP requests, one should succeed and one should fail
    this->req = HTTPProtocol::HTTPRequest::Create();
    this->req->SetURL("http:///httptest.txt");
    this->httpClient->Put(this->req);

    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
HTTPClientApp::OnRunning() {
    // pump the HTTP client's message loop
    this->httpClient->DoWork();

    // check the pending request status
    if (this->req && this->req->Handled()) {
        if (this->req->GetResponse()->GetStatus() == IOStatus::OK) {
            Log::Info("SUCCESS FOR '%s'\n", this->req->GetURL().AsCStr());
            const Ptr<Stream>& data = this->req->GetResponse()->GetBody();
            data->Open(OpenMode::ReadOnly);
            String content((const char*)data->MapRead(nullptr), 0, data->Size());
            Log::Info("BODY: %s\n", content.AsCStr());
            data->Close();
        }
        else {
            Log::Info("FAIL FOR '%s'\n", this->req->GetURL().AsCStr());
        }
        this->req = nullptr;
    }

    // sleep a bit...
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return this->req ? AppState::Running : AppState::Cleanup;
}

//------------------------------------------------------------------------------
AppState::Code
HTTPClientApp::OnCleanup() {
    // cleanup everything
    Log::Info("Done!\n\n");
    this->httpClient = nullptr;
    return AppState::Destroy;
}
