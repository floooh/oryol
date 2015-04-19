//------------------------------------------------------------------------------
//  pnaclInstance.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "pnaclInstance.h"
#include "Core/Assertion.h"
#include "Core/App.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/var.h"
#include "ppapi/lib/gl/gles2/gl2ext_ppapi.h"

// externally provided Oryol main function
void PNaclAppCreator();

namespace Oryol {
namespace _priv {
    
pnaclInstance* pnaclInstance::self = nullptr;

//------------------------------------------------------------------------------
pnaclInstance::pnaclInstance(PP_Instance inst) :
pp::Instance(inst),
callbackFactory(this),
width(0),
height(0),
app(nullptr) {
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
pnaclInstance::~pnaclInstance() {
    o_assert(nullptr != self);
    self = nullptr;
}

//------------------------------------------------------------------------------
bool
pnaclInstance::HasInstance() {
    return (nullptr != pnaclInstance::self);
}

//------------------------------------------------------------------------------
pnaclInstance*
pnaclInstance::Instance() {
    o_assert(nullptr != pnaclInstance::self);
    return pnaclInstance::self;
}

//------------------------------------------------------------------------------
bool
pnaclInstance::Init(uint32_t argc, const char* argn[], const char* argv[]) {
    if (!glInitializePPAPI(pp::Module::Get()->get_browser_interface())) {
        Log::Error("Unable to initialize GL PPAPI!\n");
        return false;
    }
    PNaclAppCreator();
    return true;
}

//------------------------------------------------------------------------------
void
pnaclInstance::DidChangeView(const pp::View& view) {
    this->width = view.GetRect().width();
    this->height = view.GetRect().height();
    if (!this->context.is_null()) {
        this->context.ResizeBuffers(this->width, this->height);
    }
    if (this->viewEventFunc) {
        this->viewEventFunc(view);
    }
}

//------------------------------------------------------------------------------
void
pnaclInstance::DidChangeFocus(bool hasFocus) {
    pp::Instance::DidChangeFocus(hasFocus);
}

//------------------------------------------------------------------------------
bool
pnaclInstance::HandleInputEvent(const pp::InputEvent& event) {
    if (this->inputEventFunc) {
        return this->inputEventFunc(event);
    }
    else {
        return pp::Instance::HandleInputEvent(event);
    }
}

//------------------------------------------------------------------------------
bool
pnaclInstance::HandleDocumentLoad(const pp::URLLoader& urlLoader) {
    // FIXME!
    return pp::Instance::HandleDocumentLoad(urlLoader);
}

//------------------------------------------------------------------------------
void
pnaclInstance::HandleMessage(const pp::Var& message) {
    // FIXME!
    pp::Instance::HandleMessage(message);
}

//------------------------------------------------------------------------------
bool
pnaclInstance::initGL(const int32_t* attribList) {
    this->context = pp::Graphics3D(this, attribList);
    if (!this->BindGraphics(this->context)) {
        Log::Error("Unable to bind 3D context!\n");
        this->context = pp::Graphics3D();
        glSetCurrentContextPPAPI(0);
        return false;
    }
    glSetCurrentContextPPAPI(this->context.pp_resource());
    return true;
}

//------------------------------------------------------------------------------
void
pnaclInstance::startMainLoop(App* app_) {
    o_assert(nullptr == this->app);
    o_assert(nullptr != app_);
    this->app = app_;
    this->doOneFrame(0);
}

//------------------------------------------------------------------------------
void
pnaclInstance::doOneFrame(int32_t) {
    o_assert(nullptr != this->app);
    o_assert(!this->context.is_null());
    this->app->onFrame();
    this->flushMessages();
    this->context.SwapBuffers(this->callbackFactory.NewCallback(&pnaclInstance::doOneFrame));
}

//------------------------------------------------------------------------------
int32
pnaclInstance::GetCanvasWidth() const {
    return this->width;
}

//------------------------------------------------------------------------------
int32
pnaclInstance::GetCanvasHeight() const {
    return this->height;
}

//------------------------------------------------------------------------------
/**
    NOTE: this method can be called from any thread.
*/
void
pnaclInstance::putMsg(const char* str) {
    this->msgLock.LockWrite();
    this->msgQueue.Enqueue(pp::Var(str));
    this->msgLock.UnlockWrite();
}

//------------------------------------------------------------------------------
void
pnaclInstance::flushMessages() {
    this->msgLock.LockWrite();
    Queue<pp::Var> q(std::move(this->msgQueue));
    this->msgLock.UnlockWrite();
    while (!q.Empty()) {
        pp::Var var(q.Dequeue());
        o_assert(var.is_string());
        this->PostMessage(var);
    }
}

//------------------------------------------------------------------------------
void
pnaclInstance::enableInput(std::function<bool(const pp::InputEvent&)> func) {
    this->RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE |
                             PP_INPUTEVENT_CLASS_WHEEL |
                             PP_INPUTEVENT_CLASS_KEYBOARD);
    this->inputEventFunc = func;
}

//------------------------------------------------------------------------------
void
pnaclInstance::disableInput() {
    this->RequestInputEvents(0);
    this->inputEventFunc = nullptr;
}

//------------------------------------------------------------------------------
void
pnaclInstance::enableViewEvents(std::function<bool(const pp::View&)> func) {
    this->viewEventFunc = func;
}

//------------------------------------------------------------------------------
void
pnaclInstance::disableViewEvents() {
    this->viewEventFunc = nullptr;
}

} // namespace _priv
} // namespace Oryol
