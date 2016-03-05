#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ShapeBuilder
    @ingroup Assets
    @brief shape geometry helper class
    @todo describe ShapeBuilder
*/
#include "Assets/Gfx/MeshBuilder.h"
#include "Core/Containers/Array.h"
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"

namespace Oryol {
    
class ShapeBuilder {
public:
    /// constructor
    ShapeBuilder();
    
    /// read/write access to vertex layout
    class VertexLayout Layout;
    /// random-vertex-colors flag
    bool RandomColors;
    
    /// put new transform
    ShapeBuilder& Transform(const glm::mat4& t);
    /// put new color
    ShapeBuilder& Color(const glm::vec4& c);
    /// add a box shape
    ShapeBuilder& Box(float32 w, float32 h, float32 d, int32 tiles, bool buildPrimGroup=true);
    /// add a sphere shape
    ShapeBuilder& Sphere(float32 radius, int32 slices, int32 stacks, bool buildPrimGroup=true);
    /// add a cylinder shape
    ShapeBuilder& Cylinder(float32 radius, float32 length, int32 slices, int32 stacks, bool buildPrimGroup=true);
    /// add a torus
    ShapeBuilder& Torus(float32 ringRadius, float32 radius, int32 sides, int32 rings, bool builPrimGroup=true);
    /// add a plane
    ShapeBuilder& Plane(float32 w, float32 d, int32 tiles, bool buildPrimGroup=true);
    
    /// build geometry and clear object state
    SetupAndData<MeshSetup> Build();
    
private:
    enum ShapeType {
        InvalidType,
        BoxShape,
        SphereShape,
        CylinderShape,
        TorusShape,
        PlaneShape,
    };

    struct ShapeData {
        ShapeData() : type(InvalidType), f0(0.0f), f1(0.0f), f2(0.0f), i0(0), i1(0), numVertices(0), numTris(0) { };
        
        ShapeType type;
        glm::mat4 transform;
        float32 f0, f1, f2;
        int32 i0, i1;
        glm::vec4 color;
        int32 numVertices;
        int32 numTris;
    };

    /// update number of vertices and triangles in shape
    void UpdateNumElements(ShapeData& shapeData);
    /// helper method: build vertex colors
    void BuildVertexColors(const ShapeData& shape, int32 startVertexIndex);
    /// build box vertices and indices
    void BuildBox(const ShapeData& shape, int32 curVertexIndex, int32 curTriIndex);
    /// build sphere vertices and indices
    void BuildSphere(const ShapeData& shape, int32 curVertexIndex, int32 curTriIndex);
    /// build cylinder vertices and indices
    void BuildCylinder(const ShapeData& shape, int32 curVertexIndex, int32 curTriIndex);
    /// build torus vertices and indices
    void BuildTorus(const ShapeData& shape, int32 curVertexIndex, int32 curTriIndex);
    /// build plane vertices and indices
    void BuildPlane(const ShapeData& shape, int32 curVertexIndex, int32 curTriIndex);
    /// build a primitive group
    void buildPrimitiveGroup();
    
    int32 curPrimGroupBaseElement;
    int32 curPrimGroupNumElements;
    glm::mat4 transform;
    glm::vec4 color;
    Array<ShapeData> shapes;
    MeshBuilder meshBuilder;
};
    
} // namespace Oryol
