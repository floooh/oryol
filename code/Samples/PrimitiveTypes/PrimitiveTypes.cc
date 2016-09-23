//------------------------------------------------------------------------------
//  PrimitiveTypes.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
#include "Assets/Gfx/MeshBuilder.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class PrimitiveTypesApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    static const int NumX = 32;
    static const int NumY = 32;
    static const int NumVertices = NumX * NumY;
    static const int NumLines = NumX * (NumY-1);

    Id vertexMesh;            // mesh object with 2D grid vertex data
    StaticArray<DrawState, PrimitiveType::NumPrimitiveTypes> drawStates;
    int curPrimType = 0;
    glm::mat4 view;
    glm::mat4 proj;
    float angleX = 0.0f;
    float angleY = 0.0f;    
    Shader::Params vsParams;
};
OryolMain(PrimitiveTypesApp);

//------------------------------------------------------------------------------
AppState::Code
PrimitiveTypesApp::OnInit() {
    auto gfxSetup = GfxSetup::WindowMSAA4(640, 480, "Oryol PrimitiveTypes Test");
    Gfx::Setup(gfxSetup);
    Dbg::Setup();
    Input::Setup();

    // create a 2D vertex grid mesh, the same vertex data is combined
    // with different index buffers
    MeshBuilder meshBuilder;
    meshBuilder.NumVertices = NumVertices;
    meshBuilder.IndicesType = IndexType::None;
    meshBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Color0, VertexFormat::UByte4N);
    meshBuilder.Begin();
    const float dx = 1.0f / NumX;
    const float dy = 1.0f / NumY;
    const float xOffset = -dx * (NumX/2);
    const float yOffset = -dy * (NumY/2);
    for (int y = 0, vi=0; y < NumY; y++) {
        for (int x = 0; x < NumX; x++, vi++) {
            meshBuilder.Vertex(vi, VertexAttr::Position, x*dx+xOffset, y*dy+yOffset, 0.0f);
            switch (vi % 3) {
                case 0: meshBuilder.Vertex(vi, VertexAttr::Color0, 1.0f, 0.0f, 0.0f, 1.0f); break;
                case 1: meshBuilder.Vertex(vi, VertexAttr::Color0, 0.0f, 1.0f, 0.0f, 1.0f); break;
                default: meshBuilder.Vertex(vi, VertexAttr::Color0, 1.0f, 1.0f, 0.0f, 1.0f); break;
            }
        }
    }
    this->vertexMesh = Gfx::CreateResource(meshBuilder.Build());

    Id shd = Gfx::CreateResource(Shader::Setup());

    // point list (only need a pipeline object, no index buffer)
    {
        auto pipSetup = PipelineSetup::FromLayoutAndShader(meshBuilder.Layout, shd);
        pipSetup.DepthStencilState.DepthWriteEnabled = true;
        pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
        pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
        pipSetup.PrimType = PrimitiveType::Points;
        auto& ds = this->drawStates[PrimitiveType::Points];
        ds.Pipeline = Gfx::CreateResource(pipSetup);
        ds.Mesh[0] = this->vertexMesh;
    }

    // line list index buffer mesh and pipeline state
    {
        const int numIndices = 2 * NumLines;
        StaticArray<uint16_t,numIndices> indices;
        int i = 0;
        for (int y = 0; y < (NumY-1); y++) {
            for (int x = 0; x < NumX; x++) {
                uint16_t i0, i1;
                if (x & 1) {
                    i0 = y * NumX + x - 1;
                    i1 = i0 + NumX + 1;
                }
                else {
                    i0 = y * NumX + x + 1;
                    i1 = i0 + NumX - 1;
                }
                o_assert_dbg((i0 <= NumVertices) && (i1 <= NumVertices));
                indices[i++] = i0;
                indices[i++] = i1;
            }
        }
        o_assert_dbg(i == numIndices);
        auto mshSetup = MeshSetup::FromData(Usage::InvalidUsage, Usage::Immutable);
        mshSetup.NumVertices = 0;
        mshSetup.NumIndices = numIndices;
        mshSetup.IndicesType = IndexType::Index16;
        mshSetup.DataVertexOffset = InvalidIndex;
        mshSetup.DataIndexOffset = 0;
        auto pipSetup = PipelineSetup::FromShader(shd);
        pipSetup.DepthStencilState.DepthWriteEnabled = true;
        pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
        pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
        pipSetup.Layouts[1] = meshBuilder.Layout;
        pipSetup.PrimType = PrimitiveType::Lines;
        auto& ds = this->drawStates[PrimitiveType::Lines];
        ds.Pipeline = Gfx::CreateResource(pipSetup);
        ds.Mesh[0] = Gfx::CreateResource(mshSetup, &indices[0], numIndices*2);
        ds.Mesh[1] = this->vertexMesh;
    }

    // line-strip index buffer mesh and pipeline state
    {
        const int numIndices = NumLines;
        StaticArray<uint16_t,numIndices> indices;
        int i = 0;
        for (int y = 0; y < (NumY-1); y++) {
            for (int x = 0; x < NumX; x++) {
                const uint16_t i0 = (x & 1) ? y*NumX+x : (y+1)*NumX+x;
                o_assert_dbg(i0 <= NumVertices);
                indices[i++] = i0;
            }
        }
        o_assert_dbg(i == numIndices);
        auto mshSetup = MeshSetup::FromData(Usage::InvalidUsage, Usage::Immutable);
        mshSetup.NumVertices = 0;
        mshSetup.NumIndices = numIndices;
        mshSetup.IndicesType = IndexType::Index16;
        mshSetup.DataVertexOffset = InvalidIndex;
        mshSetup.DataIndexOffset = 0;
        auto pipSetup = PipelineSetup::FromShader(shd);
        pipSetup.DepthStencilState.DepthWriteEnabled = true;
        pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
        pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
        pipSetup.Layouts[1] = meshBuilder.Layout;
        pipSetup.PrimType = PrimitiveType::LineStrip;
        auto& ds = this->drawStates[PrimitiveType::LineStrip];
        ds.Pipeline = Gfx::CreateResource(pipSetup);
        ds.Mesh[0] = Gfx::CreateResource(mshSetup, &indices[0], numIndices*2);
        ds.Mesh[1] = this->vertexMesh;
    }

    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
PrimitiveTypesApp::OnRunning() {

    this->angleY += 0.003f;
    this->angleX += 0.001f;

    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f));
    model = glm::rotate(model, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    this->vsParams.ModelViewProjection = this->proj * this->view * model;

    Gfx::ApplyDefaultRenderTarget();
    int num = 0;
    switch (this->curPrimType) {
        case PrimitiveType::Points: num = NumVertices; break;
        case PrimitiveType::Lines:  num = 2*NumLines; break;
        case PrimitiveType::LineStrip: num = NumLines; break;
        default: break;
    }
    if (num > 0) {
        Gfx::ApplyDrawState(this->drawStates[this->curPrimType]);
        Gfx::ApplyUniformBlock(this->vsParams);
        Gfx::Draw(PrimitiveGroup(0, num));
    }

    // handle input
    if (Input::KeyboardAttached()) {
        if (Input::KeyDown(Key::N1)) {
            this->curPrimType = PrimitiveType::Points;
        }
        else if (Input::KeyDown(Key::N2)) {
            this->curPrimType = PrimitiveType::Lines;
        }
        else if (Input::KeyDown(Key::N3)) {
            this->curPrimType = PrimitiveType::LineStrip;
        }
        else if (Input::KeyDown(Key::N4)) {
            this->curPrimType = PrimitiveType::Triangles;
        }
        else if (Input::KeyDown(Key::N5)) {
            this->curPrimType = PrimitiveType::TriangleStrip;
        }
    }
    if (Input::MouseAttached()) {
        if (Input::MouseButtonDown(MouseButton::Left)) {
            this->curPrimType = (this->curPrimType + 1) % PrimitiveType::NumPrimitiveTypes;
        }
    }
    if (Input::TouchpadAttached()) {
        if (Input::TouchTapped()) {
            this->curPrimType = (this->curPrimType + 1) % PrimitiveType::NumPrimitiveTypes;
        }
    }

    // print help- and status-text
    Dbg::TextColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    Dbg::Print("\n Press keys 1..5, left mouse button, or touch-tap!\n\n\r");
    for (int i = 0; i < int(PrimitiveType::NumPrimitiveTypes); i++) {
        if (i == this->curPrimType) {
            Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else {
            Dbg::TextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        }
        Dbg::PrintF("\n\r %d: %s", i+1, PrimitiveType::ToString(PrimitiveType::Code(i)));
    }

    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();

    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
PrimitiveTypesApp::OnCleanup() {
    Input::Discard();
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}


