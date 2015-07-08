//------------------------------------------------------------------------------
//  imguiWrapper.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "imguiWrapper.h"
#include "Core/Assertion.h"
#include "Input/Input.h"
#include "IMUIShaders.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Oryol {
namespace _priv {


imguiWrapper* imguiWrapper::self = nullptr;

//------------------------------------------------------------------------------
void
imguiWrapper::Setup() {
    o_assert_dbg(!this->IsValid());
    self = this;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

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

    // create gfx resources
    this->resLabel = Gfx::PushResourceLabel();
    this->setupFontTexture();
    this->setupMesh();
    this->setupDrawState();
    Gfx::PopResourceLabel();

    this->isValid = true;
}

//------------------------------------------------------------------------------
void
imguiWrapper::Discard() {
    o_assert_dbg(this->IsValid());
    ImGui::GetIO().Fonts->TexID = 0;
    ImGui::Shutdown();
    Gfx::DestroyResources(this->resLabel);
    this->isValid = false;
    self = nullptr;
}

//------------------------------------------------------------------------------
bool
imguiWrapper::IsValid() const {
    return this->isValid;
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

    auto texSetup = TextureSetup::FromPixelData(width, height, 1, TextureType::Texture2D, PixelFormat::L8);
    texSetup.WrapU = TextureWrapMode::ClampToEdge;
    texSetup.WrapV = TextureWrapMode::ClampToEdge;
    texSetup.MinFilter = TextureFilterMode::Nearest;
    texSetup.MagFilter = TextureFilterMode::Nearest;
    texSetup.ImageSizes[0][0] = imgSize;
    this->fontTexture = Gfx::CreateResource(texSetup, pixels, imgSize);

    // there will only be one texture
    io.Fonts->TexID = nullptr;
}


//------------------------------------------------------------------------------
void
imguiWrapper::setupMesh() {
    o_assert_dbg(!this->mesh.IsValid());

    MeshSetup setup = MeshSetup::Empty(MaxNumVertices, Usage::Stream);
    setup.Layout
        .Add(VertexAttr::Position, VertexFormat::Float2)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2)
        .Add(VertexAttr::Color0, VertexFormat::UByte4N);
    o_assert_dbg(setup.Layout.ByteSize() == sizeof(ImDrawVert));

    this->mesh = Gfx::CreateResource(setup);
    o_assert(this->mesh.IsValid());
    o_assert(Gfx::QueryResourceInfo(this->mesh).State == ResourceState::Valid);
}

//------------------------------------------------------------------------------
void
imguiWrapper::setupDrawState() {
    o_assert_dbg(!this->drawState.IsValid());
    o_assert_dbg(this->mesh.IsValid());

    Id prog = Gfx::CreateResource(Shaders::IMUIShader::CreateSetup());
    
    auto dss = DrawStateSetup::FromMeshAndProg(this->mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = false;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    dss.BlendState.BlendEnabled = true;
    dss.BlendState.SrcFactorRGB = BlendFactor::SrcAlpha;
    dss.BlendState.DstFactorRGB = BlendFactor::OneMinusSrcAlpha;
    dss.BlendState.ColorFormat = Gfx::DisplayAttrs().ColorPixelFormat;
    dss.BlendState.DepthFormat = Gfx::DisplayAttrs().DepthPixelFormat;
    dss.RasterizerState.ScissorTestEnabled = true;
    dss.RasterizerState.CullFaceEnabled = false;
    dss.RasterizerState.SampleCount = Gfx::DisplayAttrs().SampleCount;
    this->drawState = Gfx::CreateResource(dss);
}

//------------------------------------------------------------------------------
void
imguiWrapper::NewFrame(float32 frameDurationInSeconds) {

    ImGuiIO& io = ImGui::GetIO();
    DisplayAttrs dispAttrs = Gfx::RenderTargetAttrs();
    o_assert_dbg((dispAttrs.FramebufferWidth > 0) && (dispAttrs.FramebufferHeight > 0));
    io.DisplaySize = ImVec2((float)dispAttrs.FramebufferWidth, (float)dispAttrs.FramebufferHeight);
    io.DeltaTime = frameDurationInSeconds;

    // transfer input
    if (Input::IsValid()) {

        const Mouse& mouse = Input::Mouse();
        io.MousePos.x = mouse.Position.x;
        io.MousePos.y = mouse.Position.y;
        io.MouseWheel = mouse.Scroll.y;
        for (int btn = 0; btn < 3; btn++) {
            io.MouseDown[btn] = mouse.ButtonDown((Mouse::Button)btn) || mouse.ButtonPressed((Mouse::Button)btn);
        }

        const Keyboard& kbd = Input::Keyboard();
        const wchar_t* text = kbd.CapturedText();
        while (wchar_t c = *text++) {
            io.AddInputCharacter((unsigned short)c);
        }
        io.KeyCtrl  = kbd.KeyPressed(Key::LeftControl) || kbd.KeyPressed(Key::RightControl);
        io.KeyShift = kbd.KeyPressed(Key::LeftShift) || kbd.KeyPressed(Key::RightShift);
        io.KeyAlt   = kbd.KeyPressed(Key::LeftAlt) || kbd.KeyPressed(Key::RightAlt);

        const static Key::Code keys[] = {
            Key::Tab, Key::Left, Key::Right, Key::Up, Key::Down, Key::Home, Key::End,
            Key::Delete, Key::BackSpace, Key::Enter, Key::Escape,
            Key::A, Key::C, Key::V, Key::X, Key::Y, Key::Z
        };
        for (auto key : keys) {
            io.KeysDown[key] = kbd.KeyPressed(key);
        }
    }
    ImGui::NewFrame();
}

//------------------------------------------------------------------------------
void
imguiWrapper::imguiRenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count) {
    o_assert_dbg(self);

    if (cmd_lists_count == 0) {
        return;
    }

    // copy over vertices into single vertex buffer
    int numVertices = 0;
    int numCmdLists;
    for (numCmdLists = 0; numCmdLists < cmd_lists_count; numCmdLists++) {
        const ImDrawList* cmd_list = cmd_lists[numCmdLists];
        const int cmdListNumVerts = (const int) cmd_list->vtx_buffer.size();
        if ((numVertices + cmdListNumVerts) <= imguiWrapper::MaxNumVertices) {
            Memory::Copy(&(cmd_list->vtx_buffer[0]), &(self->vertexData[numVertices]), cmdListNumVerts * sizeof(ImDrawVert));
            numVertices += cmdListNumVerts;
        }
        else {
            break;
        }
    }

    // draw command lists
    Shaders::IMUIShader::VSParams vsParams;
    Shaders::IMUIShader::FSParams fsParams;
    const float width  = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    vsParams.Ortho = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    fsParams.Texture = self->fontTexture;
    const int vertexDataSize = numVertices * sizeof(ImDrawVert);

    Gfx::UpdateVertices(self->mesh, self->vertexData, vertexDataSize);
    Gfx::ApplyDrawState(self->drawState);
    Gfx::ApplyUniformBlock(vsParams);
    Gfx::ApplyUniformBlock(fsParams);
    int vtx_offset = 0;
    for (int cmdListIndex = 0; cmdListIndex < numCmdLists; cmdListIndex++) {
        const ImDrawList* cmd_list = cmd_lists[cmdListIndex];
        const ImDrawCmd* pcmd_end = cmd_list->commands.end();
        for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
        {
            if (pcmd->user_callback)
            {
                pcmd->user_callback(cmd_list, pcmd);
            }
            else
            {
                Gfx::ApplyScissorRect((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
                Gfx::Draw(PrimitiveGroup(PrimitiveType::Triangles, vtx_offset, pcmd->vtx_count));
            }
            vtx_offset += pcmd->vtx_count;
        }
    }
    Gfx::ApplyScissorRect(0, 0, (int32)ImGui::GetIO().DisplaySize.x, (int32)ImGui::GetIO().DisplaySize.y);
}

} // namespace _priv
} // namespace Oryol