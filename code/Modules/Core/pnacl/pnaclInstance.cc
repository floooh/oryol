//------------------------------------------------------------------------------
//  pnaclInstance.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "pnaclInstance.h"
#include "Core/Assert.h"
#include "Core/App.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/var.h"
#include "ppapi/lib/gl/gles2/gl2ext_ppapi.h"

// externally provided Oryol main function
void PNaclAppCreator();

namespace Oryol {
namespace Core {
    
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
    this->RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE |
                             PP_INPUTEVENT_CLASS_WHEEL |
                             PP_INPUTEVENT_CLASS_KEYBOARD);
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
    // FIXME: convert command line args
    return true;
}

//------------------------------------------------------------------------------
void
pnaclInstance::DidChangeView(const pp::View& view) {
    int32_t newWidth = view.GetRect().width();
    int32_t newHeight = view.GetRect().height();

    if (this->context.is_null()) {
        // first call, setup everything and call the Oryol main function
        this->width = newWidth;
        this->height = newHeight;
        if (!this->initGL()) {
            // GL initialization failed
            return;
        }
        PNaclAppCreator();
    }
    else {
        // FIXME: a resize happened
        this->width = newWidth;
        this->height = newHeight;
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
    // FIXME!
    return pp::Instance::HandleInputEvent(event);
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
pnaclInstance::initGL() {
    o_assert((this->width > 0) && (this->height > 0));

    if (!glInitializePPAPI(pp::Module::Get()->get_browser_interface())) {
        Log::Error("Unable to initial GL PPAPI!\n");
        return false;
    }
    const int32_t attribList[] = {
        PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 0,
        PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
        PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
        PP_GRAPHICS3DATTRIB_WIDTH, this->width,
        PP_GRAPHICS3DATTRIB_HEIGHT, this->height,
        PP_GRAPHICS3DATTRIB_NONE
    };
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
pnaclInstance::GetFramebufferWidth() const {
    return this->width;
}

//------------------------------------------------------------------------------
int32
pnaclInstance::GetFramebufferHeight() const {
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

} // namespace Core
} // namespace Oryol