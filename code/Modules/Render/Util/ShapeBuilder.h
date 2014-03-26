#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::ShapeBuilder
    @brief shape geometry helper class
    @todo describe ShapeBuilder
*/
#include "Render/Util/MeshBuilder.h"
#include "Core/Containers/Array.h"
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"

namespace Oryol {
namespace Render {
    
class ShapeBuilder {
public:
    /// add vertex component
    void AddComponent(const VertexComponent& comp);
    /// add vertex component
    void AddComponent(const VertexAttr::Code attr, VertexFormat::Code format);
    /// add a box shape
    void AddBox(const glm::mat4& transform, float32 w, float32 h, float32 d, int32 subDivs, const glm::vec4& color);
    /// add a sphere shape
    void AddSphere(const glm::mat4& transform, float32 radius, int32 slices, int32 stacks, const glm::vec4& color);
    /// add a cylinder shape
    void AddCylinder(const glm::mat4& transform, float32 radius1, float32 radius2, float32 length, int32 slices, int32 stacks, const glm::vec4& color);
    /// add a torus
    void AddTorus(const glm::mat4& transform, float32 innerRadius, float32 outerRadius, int32 sides, int32 rings, const glm::vec4& color);
    /// add a plane
    void AddPlane(const glm::mat4& transform, float32 w, float32 d, int32 subDivs, const glm::vec4& color);
    
    /// build geometry
    void Build();
    /// get the resulting data stream with vertex and index data
    const Core::Ptr<IO::Stream>& GetStream() const;
    
private:
    enum ShapeType {
        InvalidType,
        Box,
        Sphere,
        Cylinder,
        Torus,
        Plane,
    };

    struct ShapeData {
        ShapeData() : type(InvalidType), f0(0.0f), f1(0.0f), f2(0.0f), i0(0), i1(0) { };
        
        ShapeType type;
        glm::mat4 transform;
        float32 f0, f1, f2;
        int32 i0, i1;
        glm::vec4 color;
    };
    
    Core::Array<ShapeData> shapes;
    MeshBuilder meshBuilder;
};
    
} // namespace Render
} // namespace Oryol
