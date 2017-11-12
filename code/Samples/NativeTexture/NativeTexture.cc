//------------------------------------------------------------------------------
//  NativeTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"
#include <cstring>

// need to access GL API directly
#if ORYOL_OPENGL
#include "Gfx/private/gl/gl_impl.h"
#endif

using namespace Oryol;

class NativeTextureApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    AppState::Code notSupported();      // render a 'not supported' message

    glm::mat4 computeMVP(const glm::vec3& pos);
    DrawState drawState;
    ResourceLabel texLabel;
    Shader::vsParams params;
    glm::mat4 view;
    glm::mat4 proj;
    float angleX = 0.0f;
    float angleY = 0.0f;
    uint8_t counter = 0;
    static const int TexWidth = 32;
    static const int TexHeight = 32;
    uint32_t Buffer[TexWidth * TexHeight];
    #if ORYOL_OPENGL
    GLuint glTextures[2] = { };
    #endif
};
OryolMain(NativeTextureApp);

//------------------------------------------------------------------------------
AppState::Code
NativeTextureApp::OnInit() {

    auto gfxSetup = GfxSetup::WindowMSAA4(600, 400, "Oryol NativeTexture Sample");
    Gfx::Setup(gfxSetup);
    Dbg::Setup();

    // native texture handles are currently only supported on GL, on
    // other APIs, just display a warning
    if (!Gfx::QueryFeature(GfxFeature::NativeTexture)) {
        return App::OnInit();
    }

    ShapeBuilder shapeBuilder;
    shapeBuilder.RandomColors = true;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::TexCoord0, VertexFormat::Float2 }
    };
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 4);
    this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
    Id shd = Gfx::CreateResource(Shader::Setup());

    #if ORYOL_OPENGL
    // the interesting part, create 2 GL textures and hand them to the
    // Oryol Gfx module (two instead of one because we'll update the
    // textures dynamically, and Oryol rotates through them internally)
    ::glGenTextures(2, this->glTextures);
    ::glActiveTexture(GL_TEXTURE0);
    for (int i = 0; i < 2; i++) {
        ::glBindTexture(GL_TEXTURE_2D, this->glTextures[i]);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TexWidth, TexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    // after we messed directly with the native 3D API, need to clear Oryol's
    // state cache, otherwise the renderer will become confused
    Gfx::ResetStateCache();

    // make sure that the texture creation parameters here match the OpenGL
    // creation parameters (size, texture type, pixel format etc...),
    auto texSetup = TextureSetup::FromNativeTexture(TexWidth, TexHeight, 1,
        TextureType::Texture2D,
        PixelFormat::RGBA8,
        Usage::Stream,
        this->glTextures[0],
        this->glTextures[1]);
    // push a new resource label and keep it for later since we'll have
    // to cleanup the resource ourselves
    Gfx::PushResourceLabel();
    this->drawState.FSTexture[0] = Gfx::CreateResource(texSetup);
    this->texLabel = Gfx::PopResourceLabel();
    #endif
    
    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    ps.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->drawState.Pipeline = Gfx::CreateResource(ps);

    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
NativeTextureApp::OnRunning() {

    if (!Gfx::QueryFeature(GfxFeature::NativeTexture)) {
        return notSupported();
    }
    this->angleY += 0.01f;
    this->angleX += 0.02f;

    // update the dynamic texture with some random data
    for (int y = 0; y < TexHeight; y++) {
        for (int x = 0; x < TexWidth; x++) {
            uint32_t c = 0xFF000000 |
                         uint8_t(this->counter) << 16 |
                         uint8_t(this->counter * 3) << 8 |
                         uint8_t(this->counter * 23);
            this->Buffer[y * TexWidth + x] = c;
            this->counter++;
        }
    }
    this->counter++;
    ImageDataAttrs updAttrs;
    updAttrs.NumFaces = 1;
    updAttrs.NumMipMaps = 1;
    updAttrs.Offsets[0][0] = 0;
    updAttrs.Sizes[0][0] = sizeof(this->Buffer);
    Gfx::UpdateTexture(this->drawState.FSTexture[0], this->Buffer, updAttrs);

    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    this->params.mvp = this->computeMVP(glm::vec3(0.0f, 0.0f, -3.0f));
    Gfx::ApplyUniformBlock(this->params);
    Gfx::Draw();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
NativeTextureApp::OnCleanup() {
    // for externally created textures, we need to cleanup the resources
    // ourselves, Oryol will not delete the externally owned GL textures!
    #if ORYOL_OPENGL
    Gfx::DestroyResources(this->texLabel);
    ::glDeleteTextures(2, this->glTextures);
    #endif
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
glm::mat4
NativeTextureApp::computeMVP(const glm::vec3& pos) {
    glm::mat4 modelTform = glm::translate(glm::mat4(), pos);
    modelTform = glm::rotate(modelTform, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    modelTform = glm::rotate(modelTform, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    return this->proj * this->view * modelTform;
}

//------------------------------------------------------------------------------
AppState::Code
NativeTextureApp::notSupported() {
    const char* msg = "This demo needs GL\n";
    int x = (Gfx::DisplayAttrs().FramebufferWidth/16 - int(std::strlen(msg)))/2;
    int y = Gfx::DisplayAttrs().FramebufferHeight/16/2;
    Gfx::BeginPass(PassAction::Clear(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f)));
    Dbg::TextScale(2.0f, 2.0f);
    Dbg::CursorPos(uint8_t(x), uint8_t(y));
    Dbg::Print(msg);
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}
