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

    StaticArray<DrawState, PrimitiveType::Num> drawStates;
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
createIndexBuffer(const uint16_t* data, int dataSize) {
    return Gfx::CreateBuffer(BufferDesc()
        .Size(dataSize)
        .Content(data)
        .Type(BufferType::IndexBuffer)
        .Usage(Usage::Immutable));
}

//------------------------------------------------------------------------------
Id
createPipeline(PrimitiveType::Code primType, IndexType::Code indexType, const VertexLayout& layout, Id shd, int sampleCount) {
    return Gfx::CreatePipeline(PipelineDesc()
        .Shader(shd)
        .Layout(0, layout)
        .DepthWriteEnabled(true)
        .DepthCmpFunc(CompareFunc::LessEqual)
        .SampleCount(sampleCount)
        .IndexType(indexType)
        .PrimitiveType(primType));
}

//------------------------------------------------------------------------------
AppState::Code
PrimitiveTypesApp::OnInit() {
    Gfx::Setup(GfxDesc().Width(640).Height(480).SampleCount(4).Title("Oryol PrimitiveTypes Test"));
    Dbg::Setup(DbgSetup::MSAA4());
    Input::Setup();

    // create a 2D vertex grid mesh, the same vertex data is combined
    // with different index buffers
    MeshBuilder::Result mesh = MeshBuilder()
        .NumVertices(NumVertices)
        .IndexType(IndexType::None)
        .Layout({
            { "position", VertexFormat::Float3 },
            { "color0", VertexFormat::UByte4N }
        })
        .Build([](MeshBuilder& mb) {
            const float dx = 1.0f / NumX;
            const float dy = 1.0f / NumY;
            const float xOffset = -dx * (NumX/2);
            const float yOffset = -dy * (NumY/2);
            for (int y = 0, vi=0; y < NumY; y++) {
                for (int x = 0; x < NumX; x++, vi++) {
                    mb.Vertex(vi, 0, x*dx+xOffset, y*dy+yOffset, 0.0f);
                    switch (vi % 3) {
                        case 0:  mb.Vertex(vi, 1, 1.0f, 0.0f, 0.0f, 1.0f); break;
                        case 1:  mb.Vertex(vi, 1, 0.0f, 1.0f, 0.0f, 1.0f); break;
                        default: mb.Vertex(vi, 1, 1.0f, 1.0f, 0.0f, 1.0f); break;
                    }
                }
            }
        });
    Id vbuf = Gfx::CreateBuffer(mesh.VertexBufferDesc);

    // a single shader used by all pipeline objects
    Id shd = Gfx::CreateShader(Shader::Desc());

    // now setup a complete draw state (pipeline + index mesh + shared vertex mesh) for
    // each primitive type (points, lines, linestrip, triangles, trianglestrip)

    // point list (only need a pipeline object, no index buffer)
    {
        auto& ds = this->drawStates[PrimitiveType::Points];
        ds.Pipeline = createPipeline(PrimitiveType::Points, IndexType::None, mesh.Layout, shd, Gfx::Desc().SampleCount());
        ds.VertexBuffers[0] = vbuf;
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
        ds.Pipeline = createPipeline(PrimitiveType::Lines, IndexType::UInt16, mesh.Layout, shd, Gfx::Desc().SampleCount());
        ds.VertexBuffers[0] = vbuf;
        ds.IndexBuffer = createIndexBuffer(&indices[0], indices.Size()*sizeof(uint16_t));
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
        ds.Pipeline = createPipeline(PrimitiveType::LineStrip, IndexType::UInt16, mesh.Layout, shd, Gfx::Desc().SampleCount());
        ds.VertexBuffers[0] = vbuf;
        ds.IndexBuffer = createIndexBuffer(&indices[0], indices.Size()*sizeof(uint16_t));
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
        ds.Pipeline = createPipeline(PrimitiveType::Triangles, IndexType::UInt16, mesh.Layout, shd, Gfx::Desc().SampleCount());
        ds.VertexBuffers[0] = vbuf;
        ds.IndexBuffer = createIndexBuffer(&indices[0], indices.Size()*sizeof(uint16_t));
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
        ds.Pipeline = createPipeline(PrimitiveType::TriangleStrip, IndexType::UInt16, mesh.Layout, shd, Gfx::Desc().SampleCount());
        ds.VertexBuffers[0] = vbuf;
        ds.IndexBuffer = createIndexBuffer(&indices[0], indices.Size()*sizeof(uint16_t));
    }

    const float fbWidth = (const float) Gfx::DisplayAttrs().Width;
    const float fbHeight = (const float) Gfx::DisplayAttrs().Height;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::mat4();
    this->params.psize = 4.0f;

    return App::OnInit();
}

//------------------------------------------------------------------------------
static const char* primTypeToString(PrimitiveType::Code t) {
    switch (t) {
        case PrimitiveType::Points:         return "Points";
        case PrimitiveType::Lines:          return "Lines";
        case PrimitiveType::LineStrip:      return "LineStrip";
        case PrimitiveType::Triangles:      return "Triangles";
        case PrimitiveType::TriangleStrip:  return "TriangleString";
        default: return "Unknown";
    }
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
        Gfx::Draw(0, num);
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
            this->curPrimType = (this->curPrimType + 1) % PrimitiveType::Num;
        }
    }
    if (Input::TouchpadAttached()) {
        if (Input::TouchTapped()) {
            this->curPrimType = (this->curPrimType + 1) % PrimitiveType::Num;
        }
    }

    // print help- and status-text
    Dbg::TextColor(0.0f, 1.0f, 0.0f, 1.0f);
    Dbg::PrintF("\n Point Size (left/right key to change): %d\n\r", int(this->params.psize));
    Dbg::Print(" Keys 1..5, left mouse button, or touch-tap to change primitive type\n\n\r");
    for (int i = 0; i < int(PrimitiveType::Num); i++) {
        if (i == this->curPrimType) {
            Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else {
            Dbg::TextColor(1.0f, 1.0f, 0.0f, 1.0f);
        }
        Dbg::PrintF("\n\r %d: %s", i+1, primTypeToString(PrimitiveType::Code(i)));
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


