//------------------------------------------------------------------------------
//  imguiWrapper.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "imguiWrapper.h"
#include "Core/Assertion.h"
#include "Input/Input.h"
#include "IMUI/IMUIShaders.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Oryol {
namespace _priv {


imguiWrapper* imguiWrapper::self = nullptr;

static const char* imguiGetClipboardText();
static void imguiSetClipboardText(const char* text);

//------------------------------------------------------------------------------
void
imguiWrapper::Setup(const IMUISetup& setup) {
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
    ImGui::GetIO().Fonts->TexID = 0;
    ImGui::Shutdown();
    Gfx::Resource().Destroy(this->resLabel);
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


//------------------------------------------------------------------------------
void
imguiWrapper::setupMesh() {
    o_assert_dbg(!this->mesh.IsValid());

    this->vertexLayout
        .Add(VertexAttr::Position, VertexFormat::Float2)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2)
        .Add(VertexAttr::Color0, VertexFormat::UByte4N);
    o_assert_dbg(this->vertexLayout.ByteSize() == sizeof(ImDrawVert));

    MeshSetup setup = MeshSetup::Empty(MaxNumVertices, Usage::Stream);
    setup.Layout = this->vertexLayout;
    this->mesh = Gfx::Resource().Create(setup);
    o_assert(this->mesh.IsValid());
    o_assert(Gfx::Resource().QueryResourceInfo(this->mesh).State == ResourceState::Valid);
}

//------------------------------------------------------------------------------
void
imguiWrapper::setupDrawState() {
    o_assert_dbg(!this->drawState.IsValid());
    o_assert_dbg(this->mesh.IsValid());

    Id prog = Gfx::Resource().Create(Shaders::IMUIShader::CreateSetup());
    
    auto dss = DrawStateSetup::FromMeshAndProg(this->mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = false;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    dss.BlendState.BlendEnabled = true;
    dss.BlendState.SrcFactorRGB = BlendFactor::SrcAlpha;
    dss.BlendState.DstFactorRGB = BlendFactor::OneMinusSrcAlpha;
    dss.RasterizerState.ScissorTestEnabled = true;
    dss.RasterizerState.CullFaceEnabled = false;
    this->drawState = Gfx::Resource().Create(dss);
}

//------------------------------------------------------------------------------
void
imguiWrapper::NewFrame() {

    ImGuiIO& io = ImGui::GetIO();
    DisplayAttrs dispAttrs = Gfx::RenderTargetAttrs();
    o_assert_dbg((dispAttrs.FramebufferWidth > 0) && (dispAttrs.FramebufferHeight > 0));
    io.DisplaySize = ImVec2((float)dispAttrs.FramebufferWidth, (float)dispAttrs.FramebufferHeight);
    io.DeltaTime = 1.0f/60.0f;  // FIXME

    // input
    if (Input::IsValid()) {
        const Mouse& mouse = Input::Mouse();
        io.MousePos.x = mouse.Position.x;
        io.MousePos.y = mouse.Position.y;
        for (int btn = 0; btn < 3; btn++) {
            io.MouseDown[btn] = mouse.ButtonPressed((Mouse::Button)btn);
        }
    }

    // FIXME!
    // INPUT

    ImGui::NewFrame();
}

//------------------------------------------------------------------------------
void
imguiWrapper::imguiRenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count) {
    o_assert_dbg(self);

    if (cmd_lists_count == 0) {
        return;
    }

    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    const glm::mat4 ortho = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

    // copy over vertices
    int numVertices = 0;
    int numCmdLists;
    for (numCmdLists = 0; numCmdLists < cmd_lists_count; numCmdLists++) {
        const ImDrawList* cmd_list = cmd_lists[numCmdLists];
        const int cmdListNumVerts = cmd_list->vtx_buffer.size();
        if ((numVertices + cmdListNumVerts) <= imguiWrapper::MaxNumVertices) {
            Memory::Copy(&cmd_list->vtx_buffer[0], &self->vertexData[numVertices], cmdListNumVerts * sizeof(ImDrawVert));
            numVertices += cmdListNumVerts;
        }
        else {
            break;
        }
    }
    const int vertexDataSize = numVertices * sizeof(ImDrawVert);
    Gfx::UpdateVertices(self->mesh, vertexDataSize, self->vertexData);
    Gfx::ApplyDrawState(self->drawState);
    Gfx::ApplyVariable(Shaders::IMUIShader::Ortho, ortho);
    int cmd_offset = 0;
    for (int cmdListIndex = 0; cmdListIndex < numCmdLists; cmdListIndex++) {
        const ImDrawList* cmd_list = cmd_lists[cmdListIndex];
        int vtx_offset = cmd_offset;
        const ImDrawCmd* pcmd_end = cmd_list->commands.end();
        for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
        {
            if (pcmd->user_callback)
            {
                // FIXME: what's that?
                pcmd->user_callback(cmd_list, pcmd);
            }
            else
            {
                Gfx::ApplyVariable(Shaders::IMUIShader::Texture, self->fontTexture);
                Gfx::ApplyScissorRect((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
                Gfx::Draw(PrimitiveGroup(PrimitiveType::Triangles, vtx_offset, pcmd->vtx_count));
            }
            vtx_offset += pcmd->vtx_count;
        }
        cmd_offset += vtx_offset;
    }
    Gfx::ApplyScissorRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
}

//------------------------------------------------------------------------------
const char*
imguiGetClipboardText() {
    return "";
}

//------------------------------------------------------------------------------
void
imguiSetClipboardText(const char* /*text*/) {
    // empty
}

} // namespace _priv
} // namespace Oryol