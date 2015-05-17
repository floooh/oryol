#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11TextureFactory
    @ingroup _priv
    @brief D3D11 implementation of textureFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Resource/texture.h"

namespace Oryol {
namespace _priv {

class renderer;
class texture;
class displayMgr;
class texturePool;
    
class d3d11TextureFactory {
public:
    /// constructor
    d3d11TextureFactory();
    /// destructor
    ~d3d11TextureFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(class renderer* rendr, displayMgr* displayMgr, texturePool* texPool);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource
    ResourceState::Code SetupResource(texture& tex);
    /// setup with input data
    ResourceState::Code SetupResource(texture& tex, const void* data, int32 size);
    /// discard the resource
    void DestroyResource(texture& tex);
};
    
} // namespace _priv
} // namespace Oryol
