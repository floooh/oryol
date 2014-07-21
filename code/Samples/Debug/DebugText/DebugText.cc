//------------------------------------------------------------------------------
//  DebugText.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Debug/DebugFacade.h"
#include "glm/gtc/random.hpp"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Debug;

// derived application class
class DebugTextApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    void dropChar();
    void moveChars();
    void drawText();

    RenderFacade* render;
    DebugFacade* debug;
    int32 width;
    int32 height;
    uint8* buffer = nullptr;
    Core::StringBuilder strBuilder;
};
OryolMain(DebugTextApp);

//------------------------------------------------------------------------------
AppState::Code
DebugTextApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(800, 600, "Oryol DebugText Sample"));
    this->debug  = DebugFacade::CreateSingle();
    this->debug->SetTextScale(glm::vec2(2.0f, 2.0f));
    
    this->width = this->render->GetDisplayAttrs().FramebufferWidth / 16;
    this->height = this->render->GetDisplayAttrs().FramebufferHeight / 16;
    this->buffer = (uint8*) Memory::Alloc(this->width * this->height);
    Memory::Clear(this->buffer, this->width * this->height);
    
    this->strBuilder.Reserve(this->width * 2);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
//  Drop a random new character at row 0
//
void
DebugTextApp::dropChar() {
    int chr = (int) glm::linearRand(33.0f, 255.0f);
    int col = (int) glm::linearRand(0.0f, float(this->width));
    this->buffer[col] = chr;
}

//------------------------------------------------------------------------------
//  Scan character buffer from bottom to top and move characters toward
//  bottom which have an empty cell below them.
//
void
DebugTextApp::moveChars() {
    bool isFull = true;
    for (int y = this->height - 1; y >= 1; y--) {
        int yOffset = y * this->width;
        for (int x = 0; x < this->width; x++) {
            int offset = yOffset + x;
            int above  = offset - this->width;
            if (this->buffer[offset] == 0) {
                isFull = false;
                if (this->buffer[above] != 0) {
                    this->buffer[offset] = this->buffer[above];
                    this->buffer[above] = 0;
                }
            }
        }
    }
    if (isFull) {
        Memory::Clear(this->buffer, this->width * this->height);
    }
}

//------------------------------------------------------------------------------
//  Draw the character buffer into a string.
//
void
DebugTextApp::drawText() {

    glm::vec4 color[3] = {
        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
    };

    for (int y = 0; y < this->height; y++) {
        this->strBuilder.Clear();
        int yOffset = y * this->width;
        for (int x = 0; x < this->width; x++) {
            char c = this->buffer[yOffset + x];
            if (0 == c) {
                strBuilder.Append(0x09);    // cursor right
            }
            else {
                strBuilder.Append(c);
            }
        }
        strBuilder.Append("\r\n");
        this->debug->TextColor(color[y % 3]);
        this->debug->Print(strBuilder.AsCStr());
    }
}

//------------------------------------------------------------------------------
AppState::Code
DebugTextApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
    
        this->dropChar();
        this->moveChars();
        this->drawText();

        // clear, apply mesh and shader program, and draw
        this->render->ApplyState(Render::State::ClearColor, 0.5f, 0.5f, 0.5f, 0.0f);
        this->render->Clear(true, false, false);
        
        // the text buffer must be rendered within BeginFrame()/EndFrame() though...
        this->debug->DrawTextBuffer();
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DebugTextApp::OnCleanup() {
    Memory::Free(this->buffer);
    this->buffer = nullptr;
    this->render = nullptr;
    this->debug  = nullptr;
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
