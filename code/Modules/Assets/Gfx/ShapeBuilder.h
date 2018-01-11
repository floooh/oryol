#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ShapeBuilder
    @ingroup Assets
    @brief shape geometry helper class
    @todo describe ShapeBuilder
*/
#include "Assets/Gfx/MeshBuilder.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/InlineArray.h"
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"

namespace Oryol {
    
class ShapeBuilder {
public:
    /// random-vertex-colors flag
    bool RandomColors = false;

    /// add position component to vertex layout
    ShapeBuilder& AddPositions(const StringAtom& name, VertexFormat::Code fmt);
    /// add a normal component to vertex layout
    ShapeBuilder& AddNormals(const StringAtom& name, VertexFormat::Code fmt);
    /// add a texcoord component to vertex layout
    ShapeBuilder& AddTexCoords(const StringAtom& name, VertexFormat::Code fmt);
    /// add c color component to vertex layout
    ShapeBuilder& AddColors(const StringAtom& name, VertexFormat::Code fmt);
    /// put new transform
    ShapeBuilder& Transform(const glm::mat4& t);
    /// put new color
    ShapeBuilder& Color(const glm::vec4& c);
    /// add a box shape
    ShapeBuilder& Box(float w, float h, float d, int tiles, bool buildPrimGroup=true);
    /// add a sphere shape
    ShapeBuilder& Sphere(float radius, int slices, int stacks, bool buildPrimGroup=true);
    /// add a cylinder shape
    ShapeBuilder& Cylinder(float radius, float length, int slices, int stacks, bool buildPrimGroup=true);
    /// add a torus
    ShapeBuilder& Torus(float ringRadius, float radius, int sides, int rings, bool builPrimGroup=true);
    /// add a plane
    ShapeBuilder& Plane(float w, float d, int tiles, bool buildPrimGroup=true);

    /// result struct
    struct Result {
        BufferSetup VertexBufferSetup;
        BufferSetup IndexBufferSetup;
        VertexLayout Layout;
        IndexType::Code IndexType;
        Buffer Data;
        Array<PrimitiveGroup> PrimitiveGroups;
    };
    /// build geometry and clear object state
    Result Build();
    
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
        float f0, f1, f2;
        int i0, i1;
        glm::vec4 color;
        int numVertices;
        int numTris;
    };

    /// update number of vertices and triangles in shape
    void UpdateNumElements(ShapeData& shapeData);
    /// helper method: build vertex colors
    void BuildVertexColors(const ShapeData& shape, int startVertexIndex);
    /// build box vertices and indices
    void BuildBox(const ShapeData& shape, int curVertexIndex, int curTriIndex);
    /// build sphere vertices and indices
    void BuildSphere(const ShapeData& shape, int curVertexIndex, int curTriIndex);
    /// build cylinder vertices and indices
    void BuildCylinder(const ShapeData& shape, int curVertexIndex, int curTriIndex);
    /// build torus vertices and indices
    void BuildTorus(const ShapeData& shape, int curVertexIndex, int curTriIndex);
    /// build plane vertices and indices
    void BuildPlane(const ShapeData& shape, int curVertexIndex, int curTriIndex);
    /// build a primitive group
    void buildPrimitiveGroup();
    
    int curPrimGroupBaseElement = 0;
    int curPrimGroupNumElements = 0;
    int posIndex = InvalidIndex;
    int normalIndex = InvalidIndex;
    int texCoordIndex = InvalidIndex;
    int colorIndex = InvalidIndex;
    glm::mat4 transform;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    Array<ShapeData> shapes;
    Array<PrimitiveGroup> primGroups;
    MeshBuilder meshBuilder;
};
    
} // namespace Oryol
