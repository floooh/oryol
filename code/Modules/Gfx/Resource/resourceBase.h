#pragma once
#include "Resource/Core/resourceBase.h"
#include "Gfx/Setup/ShaderSetup.h"
#include "Gfx/Setup/PipelineSetup.h"
#include "Gfx/Setup/TextureSetup.h"
#include "Gfx/Setup/MeshSetup.h"
#include "Gfx/Attrs/TextureAttrs.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/Attrs/VertexBufferAttrs.h"
#include "Gfx/Attrs/IndexBufferAttrs.h"
#include "Gfx/Core/PrimitiveGroup.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shaderBase
    @ingroup _priv
    @brief shader resource base class
*/
class shaderBase : public resourceBase<ShaderSetup> { };

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBase
    @ingroup _priv
    @brief private: base class for platform-specific texture implementation
*/
class textureBase : public resourceBase<TextureSetup> {
public:
    /// texture attributes
    TextureAttrs textureAttrs;
    
    /// clear the object
    void Clear();
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::meshBase
    @ingroup _priv
    @brief private: base class for platform-specific mesh implementations
*/
class meshBase : public resourceBase<MeshSetup> {
public:
    /// vertex buffer attributes
    VertexBufferAttrs vertexBufferAttrs;
    /// index buffer attributes
    IndexBufferAttrs indexBufferAttrs;
    /// number of primitive groups
    int numPrimGroups = 0;
    /// primitive groups
    StaticArray<PrimitiveGroup, GfxConfig::MaxNumPrimGroups> primGroups;
    
    /// clear the object
    void Clear();
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pipelineBase
    @ingroup _priv
    @brief base class for pipeline implementations
*/
class shader;
class pipelineBase : public resourceBase<PipelineSetup> {
public:
    /// clear the object
    void Clear();    
    /// shader pointer
    shader* shd = nullptr;
};

} // namespace _priv
} // namespace Oryol

