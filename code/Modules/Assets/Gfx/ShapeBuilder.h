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
    /// return a new ShapeBuilder object
    static ShapeBuilder New();
    /// declare position vertex components
    ShapeBuilder& Positions(const StringAtom& name, VertexFormat::Code fmt);
    /// declare normal vertex components
    ShapeBuilder& Normals(const StringAtom& name, VertexFormat::Code fmt);
    /// declare texture coords vertex components
    ShapeBuilder& TexCoords(const StringAtom& name, VertexFormat::Code fmt);
    /// declare color 
    ShapeBuilder& Colors(const StringAtom& name, VertexFormat::Code fmt);
    /// enable random vertex colors
    ShapeBuilder& RandomColors(bool b);
    /// put new transform
    ShapeBuilder& Transform(const glm::mat4& t);
    /// put a new vertex color
    ShapeBuilder& VertexColor(const glm::vec4& c);
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
        BufferDesc VertexBufferDesc;
        BufferDesc IndexBufferDesc;
        struct PipelineDesc PipelineDesc;
        MemoryBuffer Data;
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

    void UpdateNumElements(ShapeData& shapeData);
    void BuildVertexColors(const ShapeData& shape, int startVertexIndex);
    void BuildBox(const ShapeData& shape, int curVertexIndex, int curTriIndex);
    void BuildSphere(const ShapeData& shape, int curVertexIndex, int curTriIndex);
    void BuildCylinder(const ShapeData& shape, int curVertexIndex, int curTriIndex);
    void BuildTorus(const ShapeData& shape, int curVertexIndex, int curTriIndex);
    void BuildPlane(const ShapeData& shape, int curVertexIndex, int curTriIndex);
    void buildPrimitiveGroup();

    bool randomColors = false;
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

//------------------------------------------------------------------------------
inline ShapeBuilder
ShapeBuilder::New() {
    return ShapeBuilder();
}

} // namespace Oryol
