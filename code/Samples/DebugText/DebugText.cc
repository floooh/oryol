//------------------------------------------------------------------------------
//  DebugText.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Core/String/StringBuilder.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "glm/gtc/random.hpp"

using namespace Oryol;

class DebugTextApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
    void dropChar();
    void moveChars();
    void drawText();

    int width;
    int height;
    uint8_t* buffer = nullptr;
    StringBuilder strBuilder;
};
OryolMain(DebugTextApp);

//------------------------------------------------------------------------------
AppState::Code
DebugTextApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(800, 600, "Oryol DebugText Sample");
    gfxSetup.DefaultPassAction = PassAction::Clear(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    Gfx::Setup(gfxSetup);
    DbgSetup dbgSetup;
    dbgSetup.TextScaleX = 2.0f;
    dbgSetup.TextScaleY = 2.0f;
    Dbg::Setup(dbgSetup);
    
    this->width = Gfx::DisplayAttrs().FramebufferWidth / 16;
    this->height = Gfx::DisplayAttrs().FramebufferHeight / 16;
    this->buffer = (uint8_t*) Memory::Alloc(this->width * this->height);
    Memory::Clear(this->buffer, this->width * this->height);

    this->strBuilder.Reserve(this->width * 2);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DebugTextApp::OnRunning() {

    this->dropChar();
    this->moveChars();
    this->drawText();
    
    Gfx::BeginPass();
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DebugTextApp::OnCleanup() {
    Memory::Free(this->buffer);
    this->buffer = nullptr;
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
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
    
    float color[3][4] = {
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f, 1.0 }
    };
    
    for (int y = 0; y < this->height; y++) {
        this->strBuilder.Clear();
        int yOffset = y * this->width;
        for (int x = 0; x < this->width; x++) {
            char c = this->buffer[yOffset + x];
            if (0 == c) {
                strBuilder.Append(0x20);    // replace 0 with space
            }
            else {
                strBuilder.Append(c);
            }
        }
        strBuilder.Append("\r\n");
        Dbg::TextColor(color[y%3][0], color[y%3][1], color[y%3][2], color[y%3][3]);
        Dbg::Print(strBuilder.AsCStr());
    }
}

