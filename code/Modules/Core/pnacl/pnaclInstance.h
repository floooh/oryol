#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::pnaclInstance
    @brief derived pp::Instance class for PNaCl port
*/
#include "Core/Types.h"
#include "Core/Containers/Queue.h"
#include "Core/Threading/RWLock.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/utility/completion_callback_factory.h"

namespace Oryol {
namespace Core {

class App;

class pnaclInstance : public pp::Instance {
public:
    /// constructor
    pnaclInstance(PP_Instance instance);
    /// destructor
    virtual ~pnaclInstance();
    /// return true if the pnaclInstance exists
    static bool HasInstance();
    /// get a pointer to the (unique) pnaclInstance object
    static pnaclInstance* Instance();

    /// initialization callback
    virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
    /// did change view callback
    virtual void DidChangeView(const pp::View& view);    
    /// did change focus callback
    virtual void DidChangeFocus(bool has_focus);
    /// handle an input event
    virtual bool HandleInputEvent(const pp::InputEvent& event);
    /// document-load callback
    virtual bool HandleDocumentLoad(const pp::URLLoader& url_loader);
    /// post-message callback
    virtual void HandleMessage(const pp::Var& message);

    /// get current framebuffer width
    int32 GetFramebufferWidth() const;
    /// get current framebuffer height
    int32 GetFramebufferHeight() const;

    /// this will be called by App::StartMainLoop
    void startMainLoop(App* app);
    /// put a message to the web page
    void putMsg(const char* str);

private:
    /// initialize the GL context
    bool initGL();
    /// this is the per-frame function, it will schedule itself to implement the frame loop
    void doOneFrame(int32_t dummy);
    /// flush messages (called from within doOneFrame)
    void flushMessages();

    static pnaclInstance* self;
    pp::CompletionCallbackFactory<pnaclInstance> callbackFactory;
    pp::Graphics3D context;
    int32 width;
    int32 height;
    App* app;
    RWLock msgLock;
    Queue<pp::Var> msgQueue;
};

} // namespace Core
} // namespace Oryol
