//------------------------------------------------------------------------------
//  ShapeBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ShapeBuilder.h"

namespace Oryol {
namespace Render {

using namespace Core;

//------------------------------------------------------------------------------
void
ShapeBuilder::Clear() {
    this->shapes.Clear();
    this->meshBuilder.Clear();
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddComponent(const VertexComponent& comp) {
    this->meshBuilder.AddComponent(comp);
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddComponent(VertexAttr::Code attr, VertexFormat::Code format) {
    this->meshBuilder.AddComponent(attr, format);
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddBox(const glm::mat4& tform, float32 w, float32 h, float32 d, int32 tiles, const glm::vec4& color) {
    o_assert(tiles >= 1);
    
    ShapeData shape;
    shape.type = Box;
    shape.transform = tform;
    shape.f0 = w;
    shape.f1 = h;
    shape.f2 = d;
    shape.i0 = tiles;
    shape.color = color;
    this->UpdateNumElements(shape);
    this->shapes.AddBack(shape);
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddSphere(const glm::mat4& tform, float32 radius, int32 slices, int32 stacks, const glm::vec4& color) {
    o_assert((slices >= 3) && (stacks >= 2));

    ShapeData shape;
    shape.type = Sphere;
    shape.transform = tform;
    shape.f0 = radius;
    shape.i0 = slices;
    shape.i1 = stacks;
    shape.color = color;
    this->UpdateNumElements(shape);
    this->shapes.AddBack(shape);
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddCylinder(const glm::mat4& tform, float32 radius1, float32 radius2, float32 length, int32 slices, int32 stacks, const glm::vec4& color) {
    o_assert((slices >= 3) && (stacks >= 1));

    ShapeData shape;
    shape.type = Cylinder;
    shape.transform = tform;
    shape.f0 = radius1;
    shape.f1 = radius2;
    shape.f2 = length;
    shape.i0 = slices;
    shape.i1 = stacks;
    shape.color = color;
    this->UpdateNumElements(shape);
    this->shapes.AddBack(shape);
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddTorus(const glm::mat4& tform, float32 innerRadius, float32 outerRadius, int32 sides, int32 rings, const glm::vec4& color) {
    o_assert((sides >= 3) && (rings >= 3));

    ShapeData shape;
    shape.type = Torus;
    shape.transform = tform;
    shape.f0 =innerRadius;
    shape.f1 = outerRadius;
    shape.i0 = sides;
    shape.i1 = rings;
    shape.color = color;
    this->UpdateNumElements(shape);
    this->shapes.AddBack(shape);
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddPlane(const glm::mat4& tform, float32 w, float32 d, int32 tiles, const glm::vec4& color) {
    o_assert(tiles >= 1);

    ShapeData shape;
    shape.type = Plane;
    shape.transform = tform;
    shape.f0 = w;
    shape.f1 = d;
    shape.i0 = tiles;
    shape.color = color;
    this->UpdateNumElements(shape);
    this->shapes.AddBack(shape);
}

//------------------------------------------------------------------------------
const Ptr<IO::Stream>&
ShapeBuilder::GetStream() const {
    return this->meshBuilder.GetStream();
}

//------------------------------------------------------------------------------
void
ShapeBuilder::UpdateNumElements(ShapeData& shape) {
    switch (shape.type) {
        case Box:
            {
                const int32 numTiles = shape.i0;
                shape.numVertices = (numTiles + 1) * (numTiles + 1) * 6;
                shape.numTris = numTiles * numTiles * 2 * 6;
            }
            break;
        case Sphere:
            {
                const int32 numSlices = shape.i0;
                const int32 numStacks = shape.i1;
                shape.numVertices = 2 + (numSlices + 1) * (numStacks - 1);
                shape.numTris = 2 * numSlices + 2 * numSlices * (numStacks - 2);
            }
            break;
        case Cylinder:
            {
                const int32 numSlices = shape.i0;
                const int32 numStacks = shape.i1;
                shape.numVertices = 2 * (numSlices + 1) + (numStacks + 1) * (numSlices + 1);
                shape.numTris = 2 * numSlices + 2 * numSlices * numStacks;
            }
            break;
        case Torus:
            {
                const int32 numSides = shape.i0;
                const int32 numRings = shape.i1;
                shape.numVertices = (numRings + 1) * numSides;
                shape.numTris = (numRings - 1) * numSides * 2 + numSides * 2;
            }
            break;
        case Plane:
            {
                const int32 numTiles = shape.i0;
                shape.numVertices = (numTiles + 1) * (numTiles + 1);
                shape.numTris = numTiles * numTiles * 2;
            }
            break;
        default:
            o_assert(false);
            break;
    }
}

//------------------------------------------------------------------------------
void
ShapeBuilder::Build() {
    o_assert(!this->shapes.Empty());
    
    // overall number of vertices and indices
    int32 numVerticesAll = 0;
    int32 numIndicesAll = 0;
    for (const ShapeData& shape : this->shapes) {
        numVerticesAll += shape.numVertices;
        numIndicesAll += shape.numTris * 3;
    }
    
    // configure the mesh builder
    this->meshBuilder.SetNumVertices(numVerticesAll);
    this->meshBuilder.SetIndexType(IndexType::Index16);
    this->meshBuilder.SetNumIndices(numIndicesAll);
    this->meshBuilder.AddPrimitiveGroup(PrimitiveType::Triangles, 0, numIndicesAll);
    this->meshBuilder.Begin();
    int32 curVertexIndex = 0;
    int32 curTriIndex = 0;
    for (const ShapeData& shape : this->shapes) {
        switch (shape.type) {
            case Box:
                this->BuildBox(shape, curVertexIndex, curTriIndex);
                break;
            case Sphere:
                this->BuildSphere(shape, curVertexIndex, curTriIndex);
                break;
            case Cylinder:
                this->BuildCylinder(shape, curVertexIndex, curTriIndex);
                break;
            case Torus:
                this->BuildTorus(shape, curVertexIndex, curTriIndex);
                break;
            case Plane:
                this->BuildPlane(shape, curVertexIndex, curTriIndex);
                break;
            default:
                o_assert(false);
                break;
        }
        curVertexIndex += shape.numVertices;
        curTriIndex += shape.numTris;
    }
    this->meshBuilder.End();
}

//------------------------------------------------------------------------------
void
ShapeBuilder::BuildBox(const ShapeData& shape, int32 curVertexIndex, int32 curTriIndex) {
    o_assert(this->meshBuilder.GetVertexLayout().Contains(VertexAttr::Position));
    
    const int32 startVertexIndex = curVertexIndex;
    const int32 numTiles = shape.i0;
    const int32 numVerticesPerFace = (numTiles + 1) * (numTiles + 1);
    o_assert(numVerticesPerFace == shape.numVertices / 6);
    const float32 w = shape.f0;
    const float32 h = shape.f1;
    const float32 d = shape.f2;
    //const glm::vec4& color = shape.color;
    const float32 x0 = -w * 0.5f;
    const float32 x1 = +w * 0.5f;
    const float32 y0 = -h * 0.5f;
    const float32 y1 = +h * 0.5f;
    const float32 z0 = -d * 0.5f;
    const float32 z1 = +d * 0.5f;
    const float32 dx = w / numTiles;
    const float32 dy = h / numTiles;
    const float32 dz = d / numTiles;
    
    // bottom/top plane vertices
    glm::vec4 pos;
    for (int32 topBottom = 0; topBottom < 2; topBottom++) {
        pos.y = (topBottom == 0) ? y0 : y1;
        for (int32 ix = 0; ix <= numTiles; ix++) {
            pos.x = x0 + dx * ix;
            for (int32 iz = 0; iz <= numTiles; iz++) {
                pos.z = z0 + dz * iz;
                glm::vec4 tpos = shape.transform * pos;
                this->meshBuilder.Vertex(curVertexIndex++, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
            }
        }
    }
    
    // left/right plane vertices
    for (int32 leftRight = 0; leftRight < 2; leftRight++) {
        pos.x = (leftRight == 0) ? x0 : x1;
        for (int32 iy = 0; iy <= numTiles; iy++) {
            pos.y = y0 + dy * iy;
            for (int32 iz = 0; iz <= numTiles; iz++) {
                pos.z = z0 + dz * iz;
                glm::vec4 tpos = shape.transform * pos;
                this->meshBuilder.Vertex(curVertexIndex++, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
            }
        }
    }
    
    // front/back plane vertices
    for (int32 frontBack = 0; frontBack < 2; frontBack++) {
        pos.z = (frontBack == 0) ? z0 : z1;
        for (int32 ix = 0; ix <= numTiles; ix++) {
            pos.x = x0 + dx * ix;
            for (int32 iy = 0; iy <= numTiles; iy++) {
                pos.y = y0 + dy * iy;
                glm::vec4 tpos = shape.transform * pos;
                this->meshBuilder.Vertex(curVertexIndex++, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
            }
        }
    }
    
    // FIXME: colors
    // FIXME: normals/binormals/tangents
    // FIXME: uvs
    
    // write triangle indices
    for (int32 face = 0; face < 6; face++) {
        uint16 faceStartIndex = startVertexIndex + numVerticesPerFace;
        for (int32 j = 0; j < numTiles; j++) {
            for (int32 i = 0; i < numTiles; i++) {
                // tile vertex indices
                uint16 i0 = faceStartIndex + (j * (numTiles + 1)) + i;
                uint16 i1 = i0 + 1;
                uint16 i2 = i0 + numTiles + 1;
                uint16 i3 = i2 + 1;
                
                // the 2 tile triangles
                this->meshBuilder.Triangle(curTriIndex++, i0, i1, i2);
                this->meshBuilder.Triangle(curTriIndex++, i0, i2, i3);
            }
        }
    }
}

//------------------------------------------------------------------------------
void
ShapeBuilder::BuildSphere(const ShapeData& shape, int32 curVertexIndex, int32 curIndexIndex) {
    o_error("FIXME!");
}

//------------------------------------------------------------------------------
void
ShapeBuilder::BuildCylinder(const ShapeData& shape, int32 curVertexIndex, int32 curIndexIndex) {
    o_error("FIXME!");
}

//------------------------------------------------------------------------------
void
ShapeBuilder::BuildTorus(const ShapeData& shape, int32 curVertexIndex, int32 curIndexIndex) {
    o_error("FIXME!");
}

//------------------------------------------------------------------------------
void
ShapeBuilder::BuildPlane(const ShapeData& shape, int32 curVertexIndex, int32 curIndexIndex) {
    o_error("FIXME!");
}

} // namespace Render
} // namespace Oryol