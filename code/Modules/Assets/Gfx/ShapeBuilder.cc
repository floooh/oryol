//------------------------------------------------------------------------------
//  ShapeBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Config.h"
#include "ShapeBuilder.h"
#include "glm/gtc/random.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"

namespace Oryol {

//------------------------------------------------------------------------------
ShapeBuilder::ShapeBuilder() :
RandomColors(false),
curPrimGroupBaseElement(0),
curPrimGroupNumElements(0),
color(1.0f, 1.0f, 1.0f, 1.0f) {
    // empty
}

//------------------------------------------------------------------------------
ShapeBuilder&
ShapeBuilder::Transform(const glm::mat4& m) {
    this->transform = m;
    return *this;
}

//------------------------------------------------------------------------------
ShapeBuilder&
ShapeBuilder::Color(const glm::vec4& c) {
    this->color = c;
    return *this;
}

//------------------------------------------------------------------------------
void
ShapeBuilder::buildPrimitiveGroup() {
    o_assert(this->curPrimGroupNumElements > 0);
    PrimitiveGroup primGroup(this->curPrimGroupBaseElement,
                             this->curPrimGroupNumElements);
    this->meshBuilder.PrimitiveGroups.Add(primGroup);
    this->curPrimGroupBaseElement += this->curPrimGroupNumElements;
    this->curPrimGroupNumElements = 0;
}

//------------------------------------------------------------------------------
ShapeBuilder&
ShapeBuilder::Box(float w, float h, float d, int tiles, bool buildPrimGroup) {
    o_assert(tiles >= 1);
    
    ShapeData shape;
    shape.type = BoxShape;
    shape.transform = this->transform;
    shape.f0 = w;
    shape.f1 = h;
    shape.f2 = d;
    shape.i0 = tiles;
    shape.color = this->color;
    this->UpdateNumElements(shape);
    this->shapes.Add(shape);
    this->curPrimGroupNumElements += shape.numTris * 3;
    if (buildPrimGroup) {
        this->buildPrimitiveGroup();
    }
    return *this;
}

//------------------------------------------------------------------------------
ShapeBuilder&
ShapeBuilder::Sphere(float radius, int slices, int stacks, bool buildPrimGroup) {
    o_assert((slices >= 3) && (stacks >= 2));

    ShapeData shape;
    shape.type = SphereShape;
    shape.transform = this->transform;
    shape.f0 = radius;
    shape.i0 = slices;
    shape.i1 = stacks;
    shape.color = this->color;
    this->UpdateNumElements(shape);
    this->shapes.Add(shape);
    this->curPrimGroupNumElements += shape.numTris * 3;
    if (buildPrimGroup) {
        this->buildPrimitiveGroup();
    }
    return *this;
}

//------------------------------------------------------------------------------
ShapeBuilder&
ShapeBuilder::Cylinder(float radius, float length, int slices, int stacks, bool buildPrimGroup) {
    o_assert((slices >= 3) && (stacks >= 1));

    ShapeData shape;
    shape.type = CylinderShape;
    shape.transform = this->transform;
    shape.f0 = radius;
    shape.f2 = length;
    shape.i0 = slices;
    shape.i1 = stacks;
    shape.color = this->color;
    this->UpdateNumElements(shape);
    this->shapes.Add(shape);
    this->curPrimGroupNumElements += shape.numTris * 3;
    if (buildPrimGroup) {
        this->buildPrimitiveGroup();
    }
    return *this;
}

//------------------------------------------------------------------------------
ShapeBuilder&
ShapeBuilder::Torus(float ringRadius, float radius, int sides, int rings, bool buildPrimGroup) {
    o_assert((sides >= 3) && (rings >= 3));

    ShapeData shape;
    shape.type = TorusShape;
    shape.transform = this->transform;
    shape.f0 = ringRadius;
    shape.f1 = radius;
    shape.i0 = sides;
    shape.i1 = rings;
    shape.color = this->color;
    this->UpdateNumElements(shape);
    this->shapes.Add(shape);
    this->curPrimGroupNumElements += shape.numTris * 3;
    if (buildPrimGroup) {
        this->buildPrimitiveGroup();
    }
    return *this;
}

//------------------------------------------------------------------------------
ShapeBuilder&
ShapeBuilder::Plane(float w, float d, int tiles, bool buildPrimGroup) {
    o_assert(tiles >= 1);

    ShapeData shape;
    shape.type = PlaneShape;
    shape.transform = this->transform;
    shape.f0 = w;
    shape.f1 = d;
    shape.i0 = tiles;
    shape.color = this->color;
    this->UpdateNumElements(shape);
    this->shapes.Add(shape);
    this->curPrimGroupNumElements += shape.numTris * 3;
    if (buildPrimGroup) {
        this->buildPrimitiveGroup();
    }
    return *this;
}

//------------------------------------------------------------------------------
void
ShapeBuilder::UpdateNumElements(ShapeData& shape) {
    switch (shape.type) {
        case BoxShape:
            {
                const int numTiles = shape.i0;
                shape.numVertices = (numTiles + 1) * (numTiles + 1) * 6;
                shape.numTris = numTiles * numTiles * 2 * 6;
            }
            break;
        case SphereShape:
            {
                // see BuildSphere() for geometry layout
                const int numSlices = shape.i0;
                const int numStacks = shape.i1;
                shape.numVertices = (numSlices + 1) * (numStacks + 1);
                shape.numTris = (2 * numSlices * numStacks) - (2 * numSlices);
            }
            break;
        case CylinderShape:
            {
                // see BuildCylinder() for geometry layout
                const int numSlices = shape.i0;
                const int numStacks = shape.i1;
                shape.numVertices = (numSlices + 1) * (numStacks + 5);
                shape.numTris = (2 * numSlices * numStacks) + (2 * numSlices);
            }
            break;
        case TorusShape:
            {
                // see BuildTorus() for geometry layout
                const int numSides = shape.i0;
                const int numRings = shape.i1;
                shape.numVertices = (numSides + 1) * (numRings + 1);
                shape.numTris = 2 * numSides * numRings;
            }
            break;
        case PlaneShape:
            {
                const int numTiles = shape.i0;
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
SetupAndData<MeshSetup>
ShapeBuilder::Build() {
    o_assert(!this->shapes.Empty());
    
    // build a final primitive group?
    if (this->curPrimGroupNumElements > 0) {
        this->buildPrimitiveGroup();
    }
    
    // assign vertex layout
    this->meshBuilder.Layout = this->Layout;
    
    // overall number of vertices and indices
    int numVerticesAll = 0;
    int numIndicesAll = 0;
    for (const ShapeData& shape : this->shapes) {
        numVerticesAll += shape.numVertices;
        numIndicesAll += shape.numTris * 3;
    }
    
    // configure the mesh builder
    this->meshBuilder.NumVertices = numVerticesAll;
    this->meshBuilder.IndicesType = IndexType::Index16;
    this->meshBuilder.NumIndices  = numIndicesAll;
    this->meshBuilder.Begin();
    int curVertexIndex = 0;
    int curTriIndex = 0;
    for (const ShapeData& shape : this->shapes) {
        switch (shape.type) {
            case BoxShape:
                this->BuildBox(shape, curVertexIndex, curTriIndex);
                break;
            case SphereShape:
                this->BuildSphere(shape, curVertexIndex, curTriIndex);
                break;
            case CylinderShape:
                this->BuildCylinder(shape, curVertexIndex, curTriIndex);
                break;
            case TorusShape:
                this->BuildTorus(shape, curVertexIndex, curTriIndex);
                break;
            case PlaneShape:
                this->BuildPlane(shape, curVertexIndex, curTriIndex);
                break;
            default:
                o_assert(false);
                break;
        }
        curVertexIndex += shape.numVertices;
        curTriIndex += shape.numTris;
    }
    SetupAndData<MeshSetup> result = this->meshBuilder.Build();

    // clear private data (but not config params)
    this->curPrimGroupBaseElement = 0;
    this->curPrimGroupNumElements = 0;
    this->transform = glm::mat4();
    this->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    this->shapes.Clear();
    this->meshBuilder.PrimitiveGroups.Clear();

    return result;
}

//------------------------------------------------------------------------------
void
ShapeBuilder::BuildVertexColors(const ShapeData& shape, int startVertexIndex) {
    o_assert(this->meshBuilder.Layout.Contains(VertexAttr::Color0));
    if (this->RandomColors) {
        const glm::vec3 minRand(0.0f, 0.0f, 0.0f);
        const glm::vec3 maxRand(1.0f, 1.0f, 1.0f);
        for (int i = 0; i < shape.numVertices; i++) {
            glm::vec3 rnd = glm::linearRand(minRand, maxRand);
            this->meshBuilder.Vertex(startVertexIndex + i, VertexAttr::Color0, rnd.x, rnd.y, rnd.z, 1.0f);
        }
    }
    else {
        const float r = shape.color.x;
        const float g = shape.color.y;
        const float b = shape.color.z;
        const float a = shape.color.w;
        for (int i = 0; i < shape.numVertices; i++) {
            this->meshBuilder.Vertex(startVertexIndex + i, VertexAttr::Color0, r, g, b, a);
        }
    }
}

//------------------------------------------------------------------------------
void
ShapeBuilder::BuildBox(const ShapeData& shape, int curVertexIndex, int curTriIndex) {
    const auto& vertexLayout = this->meshBuilder.Layout;
    o_assert(vertexLayout.Contains(VertexAttr::Position));
    
    const int startVertexIndex = curVertexIndex;
    const int numTiles = shape.i0;
    const int numVerticesPerFace = (numTiles + 1) * (numTiles + 1);
    o_assert(numVerticesPerFace == shape.numVertices / 6);
    const float w = shape.f0;
    const float h = shape.f1;
    const float d = shape.f2;
    const float x0 = -w * 0.5f;
    const float x1 = +w * 0.5f;
    const float y0 = -h * 0.5f;
    const float y1 = +h * 0.5f;
    const float z0 = -d * 0.5f;
    const float z1 = +d * 0.5f;
    const float dx = w / numTiles;
    const float dy = h / numTiles;
    const float dz = d / numTiles;
    const float duv = 1.0f / numTiles;
    
    const bool hasNormals = vertexLayout.Contains(VertexAttr::Normal);
    const bool hasTexCoords = vertexLayout.Contains(VertexAttr::TexCoord0);
    
    // bottom/top plane vertices
    glm::vec4 pos(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 norm;
    for (int topBottom = 0; topBottom < 2; topBottom++) {
        pos.y = (0 == topBottom) ? y0 : y1;
        if (hasNormals) {
            norm = (topBottom == 0) ? glm::vec4(0.0f, -1.0f, 0.0f, 0.0f) : glm::vec4(0.0f, +1.0f, 0.0f, 0.0f);
            norm = shape.transform * norm;
        }
        for (int ix = 0; ix <= numTiles; ix++) {
            if (0 == topBottom) {
                pos.x = x0 + dx * ix;
            }
            else {
                pos.x = x1 - dx * ix;
            }
            for (int iz = 0; iz <= numTiles; iz++) {
                pos.z = z0 + dz * iz;
                glm::vec4 tpos = shape.transform * pos;
                this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
                if (hasNormals) {
                    this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, norm.x, norm.y, norm.z);
                }
                if (hasTexCoords) {
                    this->meshBuilder.Vertex(curVertexIndex, VertexAttr::TexCoord0, ix * duv, iz * duv);
                }
                curVertexIndex++;
            }
        }
    }
    
    // left/right plane vertices
    for (int leftRight = 0; leftRight < 2; leftRight++) {
        pos.x = (0 == leftRight) ? x0 : x1;
        if (hasNormals) {
            norm = (leftRight == 0) ? glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f) : glm::vec4(+1.0f, 0.0f, 0.0f, 0.0f);
            norm = shape.transform * norm;
        }
        for (int iy = 0; iy <= numTiles; iy++) {
            if (0 == leftRight) {
                pos.y = y1 - dy * iy;
            }
            else {
                pos.y = y0 + dy * iy;
            }
            for (int iz = 0; iz <= numTiles; iz++) {
                pos.z = z0 + dz * iz;
                glm::vec4 tpos = shape.transform * pos;
                this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
                if (hasNormals) {
                    this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, norm.x, norm.y, norm.z);
                }
                if (hasTexCoords) {
                    this->meshBuilder.Vertex(curVertexIndex, VertexAttr::TexCoord0, iy * duv, iz * duv);
                }
                curVertexIndex++;
            }
        }
    }
    
    // front/back plane vertices
    for (int frontBack = 0; frontBack < 2; frontBack++) {
        pos.z = (0 == frontBack) ? z0 : z1;
        if (hasNormals) {
            norm = (frontBack == 0) ? glm::vec4(0.0, 0.0f, -1.0f, 0.0f) : glm::vec4(0.0f, 0.0f, +1.0f, 0.0f);
            norm = shape.transform * norm;
        }
        for (int ix = 0; ix <= numTiles; ix++) {
            if (0 == frontBack) {
                pos.x = x1 - dx * ix;
            }
            else {
                pos.x = x0 + dx * ix;
            }
            for (int iy = 0; iy <= numTiles; iy++) {
                pos.y = y0 + dy * iy;
                glm::vec4 tpos = shape.transform * pos;
                this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
                if (hasNormals) {
                    this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, norm.x, norm.y, norm.z);
                }
                if (hasTexCoords) {
                    this->meshBuilder.Vertex(curVertexIndex, VertexAttr::TexCoord0, ix * duv, iy * duv);
                }
                curVertexIndex++;
            }
        }
    }
    o_assert((curVertexIndex - startVertexIndex) == shape.numVertices);
    
    if (vertexLayout.Contains(VertexAttr::Color0)) {
        this->BuildVertexColors(shape, startVertexIndex);
    }
    if (vertexLayout.Contains(VertexAttr::Binormal)) {
        o_warn("FIXME: ShapeBuilder::BuildBox() binormals not implemented yet!\n");
    }
    if (vertexLayout.Contains(VertexAttr::Tangent)) {
        o_warn("FIXME: ShapeBuilder::BuildBox() tangents not implemented yet!\n");
    }
    
    // write indices
    ORYOL_UNUSED const int startTriIndex = curTriIndex;
    for (int face = 0; face < 6; face++) {
        uint16_t faceStartIndex = startVertexIndex + face * numVerticesPerFace;
        for (int j = 0; j < numTiles; j++) {
            for (int i = 0; i < numTiles; i++) {
                // tile vertex indices
                uint16_t i0 = faceStartIndex + (j * (numTiles + 1)) + i;
                uint16_t i1 = i0 + 1;
                uint16_t i2 = i0 + numTiles + 1;
                uint16_t i3 = i2 + 1;
                
                // the 2 tile triangles
                this->meshBuilder.Triangle(curTriIndex++, i0, i1, i3);
                this->meshBuilder.Triangle(curTriIndex++, i0, i3, i2);
            }
        }
    }
    o_assert((curTriIndex - startTriIndex) == shape.numTris);
}

//------------------------------------------------------------------------------
/*
    Geometry layout for spheres is as follows (for 5 slices, 4 stacks):
    
    +  +  +  +  +  +        north pole
    |\ |\ |\ |\ |\
    | \| \| \| \| \
    +--+--+--+--+--+        30 vertices (slices + 1) * (stacks + 1)
    |\ |\ |\ |\ |\ |        30 triangles (2 * slices * stacks) - (2 * slices)
    | \| \| \| \| \|        2 orphan vertices, but f*ck it
    +--+--+--+--+--+
    |\ |\ |\ |\ |\ |
    | \| \| \| \| \|
    +--+--+--+--+--+
     \ |\ |\ |\ |\ |
      \| \| \| \| \|
    +  +  +  +  +  +        south pole
*/
void
ShapeBuilder::BuildSphere(const ShapeData& shape, int curVertexIndex, int curTriIndex) {
    const auto& vertexLayout = this->meshBuilder.Layout;
    o_assert(vertexLayout.Contains(VertexAttr::Position));
    const int startVertexIndex = curVertexIndex;
    const int numSlices = shape.i0;
    const int numStacks = shape.i1;
    const float radius = shape.f0;
    const float pi = glm::pi<float>();
    const float twoPi = 2.0f * pi;
    const float du = 1.0f / numSlices;
    const float dv = 1.0f / numStacks;
    
    bool hasNormals = vertexLayout.Contains(VertexAttr::Normal);
    bool hasTexCoords = vertexLayout.Contains(VertexAttr::TexCoord0);
    
    for (int stack = 0; stack <= numStacks; stack++) {
        const float stackAngle = (pi * stack) / numStacks;
        const float sinStack = glm::sin(stackAngle);
        const float cosStack = glm::cos(stackAngle);
        for (int slice = 0; slice <= numSlices; slice++) {
            const float sliceAngle = (twoPi * slice) / numSlices;
            const float sinSlice = glm::sin(sliceAngle);
            const float cosSlice = glm::cos(sliceAngle);
            const glm::vec3 norm(sinSlice * sinStack, cosSlice * sinStack, cosStack);
            const glm::vec4 pos(norm * radius, 1.0f);
            const glm::vec4 tpos  = shape.transform * pos;
            this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
            if (hasNormals) {
                const glm::vec4 tnorm = shape.transform * glm::vec4(norm, 0.0f);
                this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, tnorm.x, tnorm.y, tnorm.z);
            }
            if (hasTexCoords) {
                this->meshBuilder.Vertex(curVertexIndex, VertexAttr::TexCoord0, du * slice, dv * stack);
            }
            curVertexIndex++;
        }
    }
    o_assert((curVertexIndex - startVertexIndex) == shape.numVertices);

    if (vertexLayout.Contains(VertexAttr::Color0)) {
        this->BuildVertexColors(shape, startVertexIndex);
    }
    if (vertexLayout.Contains(VertexAttr::Binormal)) {
        o_warn("FIXME: ShapeBuilder::BuildSphere() binormals not implemented yet!\n");
    }
    if (vertexLayout.Contains(VertexAttr::Tangent)) {
        o_warn("FIXME: ShapeBuilder::BuildSphere() tangents not implemented yet!\n");
    }
    
    // north-pole triangles
    ORYOL_UNUSED const int startTriIndex = curTriIndex;
    int rowA = startVertexIndex;
    int rowB = rowA + numSlices + 1;
    for (int slice = 0; slice < numSlices; slice++) {
        this->meshBuilder.Triangle(curTriIndex++, rowA + slice, rowB + slice, rowB + slice + 1);
    }
    
    // stack triangles
    for (int stack = 1; stack < numStacks - 1; stack++) {
        rowA = startVertexIndex + stack * (numSlices + 1);
        rowB = rowA + numSlices + 1;
        for (int slice = 0; slice < numSlices; slice++) {
            this->meshBuilder.Triangle(curTriIndex++, rowA + slice, rowB + slice + 1, rowA + slice + 1);
            this->meshBuilder.Triangle(curTriIndex++, rowA + slice, rowB + slice, rowB + slice + 1);
        }
    }
    
    // south-pole triangles
    rowA = startVertexIndex + (numStacks - 1) * (numSlices + 1);
    rowB = rowA + numSlices + 1;
    for (int slice = 0; slice < numSlices; slice++) {
        this->meshBuilder.Triangle(curTriIndex++, rowA + slice, rowB + slice + 1, rowA + slice + 1);
    }
    o_assert((curTriIndex - startTriIndex) == shape.numTris);
}

//------------------------------------------------------------------------------
/**
    Geometry for cylinders is as follows (2 stacks, 5 slices):
    
    +  +  +  +  +  +
    |\ |\ |\ |\ |\
    | \| \| \| \| \
    +--+--+--+--+--+
    +--+--+--+--+--+    42 vertices (2 wasted) (slices + 1) * (stacks + 5)
    |\ |\ |\ |\ |\ |    30 triangles (2 * slices * stacks) + (2 * slices)
    | \| \| \| \| \|
    +--+--+--+--+--+
    |\ |\ |\ |\ |\ |
    | \| \| \| \| \|
    +--+--+--+--+--+
    +--+--+--+--+--+
     \ |\ |\ |\ |\ |
      \| \| \| \| \|
    +  +  +  +  +  +
 
*/
void
ShapeBuilder::BuildCylinder(const ShapeData& shape, int curVertexIndex, int curTriIndex) {
    const auto& vertexLayout = this->meshBuilder.Layout;
    o_assert(vertexLayout.Contains(VertexAttr::Position));
    const int startVertexIndex = curVertexIndex;
    const int numSlices = shape.i0;
    const int numStacks = shape.i1;
    const float radius = shape.f0;
    const float length  = shape.f2;
    const float pi = glm::pi<float>();
    const float twoPi = 2.0f * pi;
    const bool hasNormal = vertexLayout.Contains(VertexAttr::Normal);
    
    // north cap center vertices
    glm::vec4 norm = shape.transform * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    float y = length * 0.5f;
    glm::vec4 cpos(0.0f, y, 0.0f, 1.0f);
    glm::vec4 tcpos = shape.transform * cpos;
    for (int slice = 0; slice <= numSlices; slice++) {
        this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tcpos.x, tcpos.y, tcpos.z);
        if (hasNormal) {
            this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, norm.x, norm.y, norm.z);
        }
        curVertexIndex++;
    }
    
    // top radius verticescap
    for (int slice = 0; slice <= numSlices; slice++) {
        const float sliceAngle = (twoPi * slice) / numSlices;
        const float sinSlice = glm::sin(sliceAngle);
        const float cosSlice = glm::cos(sliceAngle);
        const glm::vec4 pos(sinSlice * radius, y, cosSlice * radius, 1.0f);
        const glm::vec4 tpos = shape.transform * pos;
        this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
        if (hasNormal) {
            this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, norm.x, norm.y, norm.z);
        }
        curVertexIndex++;
    }
    
    // cylinder shaft
    float yDiff = length / numStacks;
    for (int stack = 0; stack <= numStacks; stack++) {
        y = (length * 0.5f) - yDiff * stack;
        for (int slice = 0; slice <= numSlices; slice++) {
            const float sliceAngle = (twoPi * slice) / numSlices;
            const float sinSlice = glm::sin(sliceAngle);
            const float cosSlice = glm::cos(sliceAngle);
            const glm::vec4 pos(sinSlice * radius, y, cosSlice * radius, 1.0f);
            const glm::vec4 tpos = shape.transform * pos;
            this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
            if (hasNormal) {
                glm::vec4 norm(sinSlice, 0.0f, cosSlice, 0.0f);
                norm = shape.transform * norm;
                this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, norm.x, norm.y, norm.z);
            }
            curVertexIndex++;
        }
    }
    
    // bottom cap radius vertices
    norm = shape.transform * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
    y = -length * 0.5f;
    for (int slice = 0; slice <= numSlices; slice++) {
        const float sliceAngle = (twoPi * slice) / numSlices;
        const float sinSlice = glm::sin(sliceAngle);
        const float cosSlice = glm::cos(sliceAngle);
        const glm::vec4 pos(sinSlice * radius, y, cosSlice * radius, 1.0f);
        const glm::vec4 tpos = shape.transform * pos;
        this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
        if (hasNormal) {
            this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, norm.x, norm.y, norm.z);
        }
        curVertexIndex++;
    }
    
    // bottom cap center vertices
    cpos = glm::vec4(0.0f, y, 0.0f, 1.0f);
    tcpos = shape.transform * cpos;
    for (int slice = 0; slice <= numSlices; slice++) {
        this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tcpos.x, tcpos.y, tcpos.z);
        if (hasNormal) {
            this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, norm.x, norm.y, norm.z);
        }
        curVertexIndex++;
    }
    o_assert((curVertexIndex - startVertexIndex) == shape.numVertices);
    
    if (vertexLayout.Contains(VertexAttr::Color0)) {
        this->BuildVertexColors(shape, startVertexIndex);
    }
    if (vertexLayout.Contains(VertexAttr::Binormal)) {
        o_warn("FIXME: ShapeBuilder::BuildCylinder() binormals not implemented yet!\n");
    }
    if (vertexLayout.Contains(VertexAttr::Tangent)) {
        o_warn("FIXME: ShapeBuilder::BuildCylinder() tangents not implemented yet!\n");
    }
    if (vertexLayout.Contains(VertexAttr::TexCoord0)) {
        o_warn("FIXME: ShapeBuilder::BuildCylinder() texcoord not implemented yet!\n");
    }
    
    // north cap triangles
    ORYOL_UNUSED const int startTriIndex = curTriIndex;
    int rowA = startVertexIndex;
    int rowB = rowA + numSlices + 1;
    for (int slice = 0; slice < numSlices; slice++) {
        this->meshBuilder.Triangle(curTriIndex++, rowA + slice, rowB + slice + 1, rowB + slice);
    }
    
    // shaft triangles
    for (int stack = 0; stack < numStacks; stack++) {
        rowA = startVertexIndex + (stack + 2) * (numSlices + 1);
        rowB = rowA + numSlices + 1;
        for (int slice = 0; slice < numSlices; slice++) {
            this->meshBuilder.Triangle(curTriIndex++, rowA + slice, rowA + slice + 1, rowB + slice + 1);
            this->meshBuilder.Triangle(curTriIndex++, rowA + slice, rowB + slice + 1, rowB + slice);
        }
    }
    
    // south cap triangles
    rowA = startVertexIndex + (numStacks + 3) * (numSlices + 1);
    rowB = rowA + numSlices + 1;
    for (int slice = 0; slice < numSlices; slice++) {
        this->meshBuilder.Triangle(curTriIndex++, rowA + slice, rowA + slice + 1, rowB + slice + 1);
    }
    o_assert((curTriIndex - startTriIndex) == shape.numTris);
}

//------------------------------------------------------------------------------
/**
    Geometry layout for torus (sides = 4, rings = 5):
    
    +--+--+--+--+--+
    |\ |\ |\ |\ |\ |
    | \| \| \| \| \|
    +--+--+--+--+--+    30 vertices (sides + 1) * (rings + 1)
    |\ |\ |\ |\ |\ |    40 triangles (2 * sides * rings)
    | \| \| \| \| \|
    +--+--+--+--+--+
    |\ |\ |\ |\ |\ |
    | \| \| \| \| \|
    +--+--+--+--+--+
    |\ |\ |\ |\ |\ |
    | \| \| \| \| \|
    +--+--+--+--+--+

*/
void
ShapeBuilder::BuildTorus(const ShapeData& shape, int curVertexIndex, int curTriIndex) {
    const auto& vertexLayout = this->meshBuilder.Layout;
    o_assert(vertexLayout.Contains(VertexAttr::Position));
    const int startVertexIndex = curVertexIndex;
    static const float ringRadius = shape.f0;
    static const float radius = shape.f1;
    static const int numSides = shape.i0;
    static const int numRings = shape.i1;
    const float pi = glm::pi<float>();
    const float twoPi = 2.0f * pi;
    const bool hasNormals = vertexLayout.Contains(VertexAttr::Normal);
    
    // vertex positions
    for (int side = 0; side <= numSides; side++) {
        const float phi = (side * twoPi) / numSides;
        const float sinPhi = glm::sin(phi);
        const float cosPhi = glm::cos(phi);
        for (int ring = 0; ring <= numRings; ring++) {
            const float theta = (ring * twoPi) / numRings;
            const float sinTheta = glm::sin(theta);
            const float cosTheta = glm::cos(theta);
            
            // surface position
            const float spx = cosTheta * (radius + (ringRadius * cosPhi));
            const float spy = -sinTheta * (radius + (ringRadius * cosPhi));
            const float spz = sinPhi * ringRadius;

            // surface position
            const glm::vec4 tpos = shape.transform * glm::vec4(spx, spy, spz, 1.0f);
            this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
            
            // surface normal
            if (hasNormals) {
                // inner radius position
                const float ipx = cosTheta * radius;
                const float ipy = -sinTheta * radius;
                const float ipz = 0.0f;
                glm::vec4 norm  = glm::normalize(glm::vec4(spx - ipx, spy - ipy, spz - ipz, 0.0f));
                norm = shape.transform * norm;
                this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, norm.x, norm.y, norm.z);
            }
            curVertexIndex++;
        }
    }
    o_assert((curVertexIndex - startVertexIndex) == shape.numVertices);
    
    if (vertexLayout.Contains(VertexAttr::Color0)) {
        this->BuildVertexColors(shape, startVertexIndex);
    }
    if (vertexLayout.Contains(VertexAttr::Binormal)) {
        o_warn("FIXME: ShapeBuilder::BuildTorus() binormals not implemented yet!\n");
    }
    if (vertexLayout.Contains(VertexAttr::Tangent)) {
        o_warn("FIXME: ShapeBuilder::BuildTorus() tangents not implemented yet!\n");
    }
    if (vertexLayout.Contains(VertexAttr::TexCoord0)) {
        o_warn("FIXME: ShapeBuilder::BuildTorus() texcoord not implemented yet!\n");
    }
    
    // triangles
    ORYOL_UNUSED const int startTriIndex = curTriIndex;
    for (int side = 0; side < numSides; side++) {
        const int rowA = startVertexIndex + side * (numRings + 1);
        const int rowB = rowA + numRings + 1;
        for (int ring = 0; ring < numRings; ring++) {
            this->meshBuilder.Triangle(curTriIndex++, rowA + ring, rowA + ring + 1, rowB + ring + 1);
            this->meshBuilder.Triangle(curTriIndex++, rowA + ring, rowB + ring + 1, rowB + ring);
        }
    }
    o_assert((curTriIndex - startTriIndex) == shape.numTris);
}

//------------------------------------------------------------------------------
/**
    Geometry layout for plane (4 tiles):

    +--+--+--+--+
    |\ |\ |\ |\ |
    | \| \| \| \|
    +--+--+--+--+    25 vertices (tiles + 1) * (tiles + 1)
    |\ |\ |\ |\ |    32 triangles (tiles + 1) * (tiles + 1) * 2
    | \| \| \| \|
    +--+--+--+--+
    |\ |\ |\ |\ |
    | \| \| \| \|
    +--+--+--+--+
    |\ |\ |\ |\ |
    | \| \| \| \|
    +--+--+--+--+
*/
void
ShapeBuilder::BuildPlane(const ShapeData& shape, int curVertexIndex, int curTriIndex) {
    const auto& vertexLayout = this->meshBuilder.Layout;
    o_assert(vertexLayout.Contains(VertexAttr::Position));
    const int startVertexIndex = curVertexIndex;

    const int numTiles = shape.i0;
    const float w = shape.f0;
    const float d = shape.f1;
    const float x0 = -w * 0.5f;
    const float z0 = d * 0.5f;
    const float dx = w / numTiles;
    const float dz = -d / numTiles;
    const float duv = 1.0f / numTiles;
    
    const bool hasNormal = vertexLayout.Contains(VertexAttr::Normal);
    const bool hasTexCoords = vertexLayout.Contains(VertexAttr::TexCoord0);
    
    // vertices
    glm::vec4 pos(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 norm = shape.transform * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    for (int ix = 0; ix <= numTiles; ix++) {
        pos.x = x0 + dx * ix;
        for (int iz = 0; iz <= numTiles; iz++) {
            pos.z = z0 + dz * iz;
            glm::vec4 tpos = shape.transform * pos;
            this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Position, tpos.x, tpos.y, tpos.z);
            if (hasNormal) {
                this->meshBuilder.Vertex(curVertexIndex, VertexAttr::Normal, norm.x, norm.y, norm.z);
            }
            if (hasTexCoords) {
                this->meshBuilder.Vertex(curVertexIndex, VertexAttr::TexCoord0, ix * duv, iz * duv);
            }
            curVertexIndex++;
        }
    }
    o_assert((curVertexIndex - startVertexIndex) == shape.numVertices);
    
    if (vertexLayout.Contains(VertexAttr::Color0)) {
        this->BuildVertexColors(shape, startVertexIndex);
    }
    if (vertexLayout.Contains(VertexAttr::Binormal)) {
        o_warn("FIXME: ShapeBuilder::BuildPlane() binormals not implemented yet!\n");
    }
    if (vertexLayout.Contains(VertexAttr::Tangent)) {
        o_warn("FIXME: ShapeBuilder::BuildPlane() tangents not implemented yet!\n");
    }

    // write indices
    ORYOL_UNUSED const int startTriIndex = curTriIndex;
    for (int j = 0; j < numTiles; j++) {
        for (int i = 0; i < numTiles; i++) {
            // tile vertex indices
            uint16_t i0 = startVertexIndex + (j * (numTiles + 1)) + i;
            uint16_t i1 = i0 + 1;
            uint16_t i2 = i0 + numTiles + 1;
            uint16_t i3 = i2 + 1;
            
            // the 2 tile triangles
            this->meshBuilder.Triangle(curTriIndex++, i0, i1, i3);
            this->meshBuilder.Triangle(curTriIndex++, i0, i3, i2);
        }
    }
    o_assert((curTriIndex - startTriIndex) == shape.numTris);
}

} // namespace Oryol