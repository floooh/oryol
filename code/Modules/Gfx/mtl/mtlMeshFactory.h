#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlMeshFactory
    @ingroup _priv
    @brief Metal implementation of meshFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
namespace _priv {

class renderer;
class meshPool;
class mesh;

class mtlMeshFactory {
public:
    /// constructor
    mtlMeshFactory();
    /// destructor
    ~mtlMeshFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(renderer* rendr, meshPool* mshPool);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource
    ResourceState::Code SetupResource(mesh& mesh);
    /// setup with 'raw' data
    ResourceState::Code SetupResource(mesh& mesh, const void* data, int32 size);
    /// discard the resource
    void DestroyResource(mesh& mesh);

private:
    class renderer* renderer;
    class meshPool* meshPool;
    bool isValid;
};

} // namespace _priv
} // namespace Oryol
