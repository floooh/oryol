#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/Resource/resourceBase.h
    @brief Gfx module resource classes
*/
#include "Resource/ResourceBase.h"
#include "Gfx/GfxTypes.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::shaderBase
    @ingroup _priv
    @brief shader resource base class
*/
class shaderBase : public ResourceBase {
public:
    /// the original setup object
    ShaderSetup Setup;
    /// clear the object
    void Clear();    
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBase
    @ingroup _priv
    @brief base class for platform-specific texture implementation
*/
class textureBase : public ResourceBase {
public:
    /// the original setup object
    TextureSetup Setup;
    /// texture attributes
    TextureAttrs textureAttrs;
    /// was created from native texture handles
    bool nativeHandles = false;
    /// clear the object
    void Clear();
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::meshBase
    @ingroup _priv
    @brief base class for platform-specific mesh implementations
*/
class meshBase : public ResourceBase {
public:
    /// the original setup object
    MeshSetup Setup;
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
class pipelineBase : public ResourceBase {
public:
    /// the original setup object
    PipelineSetup Setup;
    /// shader pointer
    shader* shd = nullptr;
    /// clear the object
    void Clear();    
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::renderPassBase
    @ingroup _priv
    @brief base class for render-pass implementations
*/
class texture;
class renderPassBase : public ResourceBase {
public:
    /// constructor
    renderPassBase() { 
        colorTextures.Fill(nullptr);
    }
    /// the original setup object
    PassSetup Setup;
    /// color texture pointers
    StaticArray<texture*, GfxConfig::MaxNumColorAttachments> colorTextures;
    /// depth-stencil texture pointer
    texture* depthStencilTexture = nullptr;
    /// clear the object
    void Clear();
};

} // namespace _priv
} // namespace Oryol

