//------------------------------------------------------------------------------
//  imguiWrapper.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "imguiWrapper.h"
#include "Core/Assertion.h"
#include "Input/Input.h"
#include "imgui.h"

namespace Oryol {
namespace _priv {

static void imguiRenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count);
static const char* imguiGetClipboardText();
static void imguiSetClipboardText(const char* text);

//------------------------------------------------------------------------------
void
imguiWrapper::Setup(const IMUISetup& setup) {
    o_assert_dbg(!this->IsValid());

    ImGuiIO& io = ImGui::GetIO();

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_Tab] = Key::Tab; 
    io.KeyMap[ImGuiKey_LeftArrow] = Key::Left;
    io.KeyMap[ImGuiKey_RightArrow] = Key::Right;
    io.KeyMap[ImGuiKey_UpArrow] = Key::Up;
    io.KeyMap[ImGuiKey_DownArrow] = Key::Down;
    io.KeyMap[ImGuiKey_Home] = Key::Home;
    io.KeyMap[ImGuiKey_End] = Key::End;
    io.KeyMap[ImGuiKey_Delete] = Key::Delete;
    io.KeyMap[ImGuiKey_Backspace] = Key::BackSpace;
    io.KeyMap[ImGuiKey_Enter] = Key::Enter;
    io.KeyMap[ImGuiKey_Escape] = Key::Escape;
    io.KeyMap[ImGuiKey_A] = Key::A;
    io.KeyMap[ImGuiKey_C] = Key::C;
    io.KeyMap[ImGuiKey_V] = Key::V;
    io.KeyMap[ImGuiKey_X] = Key::X;
    io.KeyMap[ImGuiKey_Y] = Key::Y;
    io.KeyMap[ImGuiKey_Z] = Key::Z;

    io.RenderDrawListsFn = imguiRenderDrawLists;
    io.SetClipboardTextFn = imguiSetClipboardText;
    io.GetClipboardTextFn = imguiGetClipboardText;

    // create gfx resources
    this->resLabel = Gfx::Resource().PushLabel();
    this->setupFontTexture();
    this->setupMesh();
    this->setupDrawState();
    Gfx::Resource().PopLabel();

    this->isValid = true;
}

//------------------------------------------------------------------------------
void
imguiWrapper::Discard() {
    o_assert_dbg(this->IsValid());
    Gfx::Resource().Destroy(this->resLabel);
    this->isValid = false;
}

//------------------------------------------------------------------------------
void
imguiWrapper::setupFontTexture() {
    o_assert_dbg(!this->fontTexture.IsValid());

    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    const int imgSize = width * height * sizeof(uint8);
    auto stream = MemoryStream::Create();
    stream->Open(OpenMode::WriteOnly);
    stream->Write(pixels, imgSize);
    stream->Close();

    auto texSetup = TextureSetup::FromPixelData(width, height, 1, TextureType::Texture2D, PixelFormat::L8);
    texSetup.WrapU = TextureWrapMode::ClampToEdge;
    texSetup.WrapV = TextureWrapMode::ClampToEdge;
    texSetup.MinFilter = TextureFilterMode::Nearest;
    texSetup.MagFilter = TextureFilterMode::Nearest;
    texSetup.ImageSizes[0][0] = imgSize;
    this->fontTexture = Gfx::Resource().Create(texSetup, stream);

    // there will only be one texture
    io.Fonts->TexID = nullptr;
}

} // namespace _priv
} // namespace Oryol