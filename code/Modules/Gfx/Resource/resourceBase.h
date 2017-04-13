#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/Resource/resourceBase.h
    @brief Gfx module resource classes
*/
#include "Resource/Core/resourceBase.h"
#include "Gfx/Core/GfxTypes.h"

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
    /// was created from native texture handles
    bool nativeHandles = false;
    
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
    /// primitive groups
    InlineArray<PrimitiveGroup, GfxConfig::MaxNumPrimGroups> primGroups;
    
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

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::renderPassBase
    @ingroup _priv
    @brief base class for render-pass implementations
*/
class texture;
class renderPassBase : public resourceBase<PassSetup> {
public:
    /// constructor
    renderPassBase() { 
        colorTextures.Fill(nullptr);
    }
    /// clear the object
    void Clear();

    /// color texture pointers
    StaticArray<texture*, GfxConfig::MaxNumColorAttachments> colorTextures;
    /// depth-stencil texture pointer
    texture* depthStencilTexture = nullptr;
};

} // namespace _priv
} // namespace Oryol

