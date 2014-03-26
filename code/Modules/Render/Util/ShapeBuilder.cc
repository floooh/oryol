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
ShapeBuilder::AddBox(const glm::mat4& tform, float32 w, float32 h, float32 d, int32 subDivs, const glm::vec4& color) {
    ShapeData shape;
    shape.type = Box;
    shape.transform = tform;
    shape.f0 = w;
    shape.f1 = h;
    shape.f2 = d;
    shape.i0 = subDivs;
    shape.color = color;
    this->shapes.AddBack(shape);
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddSphere(const glm::mat4& tform, float32 radius, int32 slices, int32 stacks, const glm::vec4& color) {
    ShapeData shape;
    shape.type = Sphere;
    shape.transform = tform;
    shape.f0 = radius;
    shape.i0 = slices;
    shape.i1 = stacks;
    shape.color = color;
    this->shapes.AddBack(shape);
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddCylinder(const glm::mat4& tform, float32 radius1, float32 radius2, float32 length, int32 slices, int32 stacks, const glm::vec4& color) {
    ShapeData shape;
    shape.type = Cylinder;
    shape.transform = tform;
    shape.f0 = radius1;
    shape.f1 = radius2;
    shape.f2 = length;
    shape.i0 = slices;
    shape.i1 = stacks;
    shape.color = color;
    this->shapes.AddBack(shape);
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddTorus(const glm::mat4& tform, float32 innerRadius, float32 outerRadius, int32 sides, int32 rings, const glm::vec4& color) {
    ShapeData shape;
    shape.type = Torus;
    shape.transform = tform;
    shape.f0 =innerRadius;
    shape.f1 = outerRadius;
    shape.i0 = sides;
    shape.i1 = rings;
    shape.color = color;
    this->shapes.AddBack(shape);
}

//------------------------------------------------------------------------------
void
ShapeBuilder::AddPlane(const glm::mat4& tform, float32 w, float32 d, int32 subDivs, const glm::vec4& color) {
    ShapeData shape;
    shape.type = Plane;
    shape.transform = tform;
    shape.f0 = w;
    shape.f1 = d;
    shape.i0 = subDivs;
    shape.color = color;
    this->shapes.AddBack(shape);
}

//------------------------------------------------------------------------------
const Ptr<IO::Stream>&
ShapeBuilder::GetStream() const {
    return this->meshBuilder.GetStream();
}

//------------------------------------------------------------------------------
void
ShapeBuilder::Build() {
    // FIXME!
}



} // namespace Render
} // namespace Oryol