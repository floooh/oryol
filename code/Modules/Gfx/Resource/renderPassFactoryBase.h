#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::renderPassFactoryBase
    @ingroup _priv
    @brief base class for renderPassFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class renderPass;

class renderPassFactoryBase {
public:
    /// destructor
    ~renderPassFactoryBase();

    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// setup renderpass resource
    ResourceState::Code SetupResource(renderPass& rp);
    /// destroy renderpass resource
    void DestroyResource(renderPass& rp);

protected:
    gfxPointers pointers;
    bool isValid = false;
};

} // namespace _priv
} // namespace Oryol
