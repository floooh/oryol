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
    static const int NumLineIndices = NumX * (NumY-1) * 2;
    static const int NumLineStripIndices = NumX * (NumY-1);
    static const int NumTriangleIndices = (NumX-1) * (NumY-1) * 3;
    static const int NumTriStripIndices = (NumX-1)*(NumY-1)*2 + (NumY-1)*2;

    StaticArray<DrawState, PrimitiveType::NumPrimitiveTypes> drawStates;
    int curPrimType = 0;
    glm::mat4 view;
    glm::mat4 proj;
    float angleX = 0.0f;
    float angleY = 0.0f;    
    Shader::params params;
};
OryolMain(PrimitiveTypesApp);

//------------------------------------------------------------------------------
Id
createIndexMesh(int numIndices, const void* data, int dataSize) {
    auto setup = MeshSetup::FromData(Usage::InvalidUsage, Usage::Immutable);
    setup.NumVertices = 0;
    setup.NumIndices = numIndices;
    setup.IndicesType = IndexType::Index16;
    setup.VertexDataOffset = InvalidIndex;
    setup.IndexDataOffset = 0;
    return Gfx::CreateResource(setup, data, dataSize);
}

//------------------------------------------------------------------------------
Id
createPipeline(PrimitiveType::Code primType, int layoutSlot, const VertexLayout& layout, Id shd, int sampleCount) {
    auto pipSetup = PipelineSetup::FromShader(shd);
    pipSetup.DepthStencilState.DepthWriteEnabled = true;
    pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    pipSetup.RasterizerState.SampleCount = sampleCount;
    pipSetup.Layouts[layoutSlot] = layout;
    pipSetup.PrimType = primType;
    return Gfx::CreateResource(pipSetup);
}

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
    meshBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Color0, VertexFormat::UByte4N }
    };
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
    Id vertexMesh = Gfx::CreateResource(meshBuilder.Build());

    // a single shader used by all pipeline objects
    Id shd = Gfx::CreateResource(Shader::Setup());

    // now setup a complete draw state (pipeline + index mesh + shared vertex mesh) for
    // each primitive type (points, lines, linestrip, triangles, trianglestrip)

    // point list (only need a pipeline object, no index buffer)
    {
        auto& ds = this->drawStates[PrimitiveType::Points];
        ds.Pipeline = createPipeline(PrimitiveType::Points, 0, meshBuilder.Layout, shd, gfxSetup.SampleCount);
        ds.Mesh[0] = vertexMesh;
    }

    // line list index buffer mesh and pipeline state
    {
        const int numIndices = NumLineIndices;
        StaticArray<uint16_t, numIndices> indices;
        int i = 0;
        for (int y = 0; y < (NumY-1); y++) {
            for (int x = 0; x < NumX; x++) {
                const uint16_t i0 = (x & 1) ? y*NumX+x-1 : y*NumX+x+1;
                const uint16_t i1 = (x & 1) ? i0+NumX+1  : i0+NumX-1;
                o_assert_dbg((i0 < NumVertices) && (i1 < NumVertices));
                indices[i++] = i0;
                indices[i++] = i1;
            }
        }
        o_assert_dbg(i == numIndices);
        auto& ds = this->drawStates[PrimitiveType::Lines];
        ds.Pipeline = createPipeline(PrimitiveType::Lines, 1, meshBuilder.Layout, shd, gfxSetup.SampleCount);
        ds.Mesh[0] = createIndexMesh(numIndices, &indices[0], numIndices*2);
        ds.Mesh[1] = vertexMesh;
    }

    // line-strip index buffer mesh and pipeline state
    {
        const int numIndices = NumLineStripIndices;
        StaticArray<uint16_t, numIndices> indices;
        int i = 0;
        for (int y = 0; y < (NumY-1); y++) {
            for (int x = 0; x < NumX; x++) {
                const uint16_t i0 = (x & 1) ? y*NumX+x : (y+1)*NumX+x;
                o_assert_dbg(i0 < NumVertices);
                indices[i++] = i0;
            }
        }
        o_assert_dbg(i == numIndices);
        auto& ds = this->drawStates[PrimitiveType::LineStrip];
        ds.Pipeline = createPipeline(PrimitiveType::LineStrip, 1, meshBuilder.Layout, shd, gfxSetup.SampleCount);
        ds.Mesh[0] = createIndexMesh(numIndices, &indices[0], numIndices*2);
        ds.Mesh[1] = vertexMesh;
    }

    // triangle-list index buffer and pipeline state
    {
        const int numIndices = NumTriangleIndices;
        StaticArray<uint16_t, numIndices> indices;
        int i = 0;
        for (int y = 0; y < (NumY-1); y++) {
            for (int x = 0; x < (NumX-1); x++) {
                const uint16_t i0 = x+y*NumX;
                const uint16_t i1 = (x & 1) ? i0+NumX : i0+1;
                const uint16_t i2 = (x & 1) ? i1+1 : i0+NumX;
                o_assert_dbg((i0 < NumVertices) && (i1 < NumVertices) && (i2 < NumVertices));
                indices[i++] = i0;
                indices[i++] = i1;
                indices[i++] = i2;
            }
        }
        o_assert_dbg(i == numIndices);
        auto& ds = this->drawStates[PrimitiveType::Triangles];
        ds.Pipeline = createPipeline(PrimitiveType::Triangles, 1, meshBuilder.Layout, shd, gfxSetup.SampleCount);
        ds.Mesh[0] = createIndexMesh(numIndices, &indices[0], numIndices*2);
        ds.Mesh[1] = vertexMesh;
    }

    // triangle-strip index buffer and pipeline state
    {
        const int numIndices = NumTriStripIndices;
        StaticArray<uint16_t, numIndices> indices;
        int i = 0;
        for (int y = 0; y < (NumY-1); y++) {
            uint16_t i0, i1;
            for (int x = 0; x < (NumX-1); x++) {
                i0 = x + y*NumX;
                i1 = i0 + NumX;
                o_assert_dbg((i0 < NumVertices) && (i1 < NumVertices));
                indices[i++] = i0;
                indices[i++] = i1;
            }
            // add a degenerate triangle
            indices[i++] = i1;
            indices[i++] = (y+1)*NumX;
        }
        o_assert_dbg(i == numIndices);
        auto& ds = this->drawStates[PrimitiveType::TriangleStrip];
        ds.Pipeline = createPipeline(PrimitiveType::TriangleStrip, 1, meshBuilder.Layout, shd, gfxSetup.SampleCount);
        ds.Mesh[0] = createIndexMesh(numIndices, &indices[0], numIndices*2);
        ds.Mesh[1] = vertexMesh;
    }

    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    this->params.psize = 4.0f;

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
PrimitiveTypesApp::OnRunning() {

    // setup the animated mvp matrix
    this->angleY += 0.003f;
    this->angleX += 0.001f;
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -1.5f));
    model = glm::rotate(model, this->angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, this->angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    this->params.mvp = this->proj * this->view * model;

    // render the currently selected drawstate
    Gfx::BeginPass();
    int num = 0;
    switch (this->curPrimType) {
        case PrimitiveType::Points: num = NumVertices; break;
        case PrimitiveType::Lines:  num = NumLineIndices; break;
        case PrimitiveType::LineStrip: num = NumLineStripIndices; break;
        case PrimitiveType::Triangles: num = NumTriangleIndices; break;
        case PrimitiveType::TriangleStrip: num = NumTriStripIndices; break;
        default: break;
    }
    if (num > 0) {
        Gfx::ApplyDrawState(this->drawStates[this->curPrimType]);
        Gfx::ApplyUniformBlock(this->params);
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
        if (Input::KeyDown(Key::Right)) {
            this->params.psize += 1.0f;
        }
        if (Input::KeyDown(Key::Left)) {
            this->params.psize -= 1.0f;
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
    Dbg::TextColor(0.0f, 1.0f, 0.0f, 1.0f);
    Dbg::PrintF("\n Point Size (left/right key to change): %d\n\r", int(this->params.psize));
    Dbg::Print(" Keys 1..5, left mouse button, or touch-tap to change primitive type\n\n\r");
    for (int i = 0; i < int(PrimitiveType::NumPrimitiveTypes); i++) {
        if (i == this->curPrimType) {
            Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else {
            Dbg::TextColor(1.0f, 1.0f, 0.0f, 1.0f);
        }
        Dbg::PrintF("\n\r %d: %s", i+1, PrimitiveType::ToString(PrimitiveType::Code(i)));
    }

    Dbg::DrawTextBuffer();
    Gfx::EndPass();
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


