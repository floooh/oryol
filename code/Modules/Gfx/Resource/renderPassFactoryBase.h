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
    /// return true if factory is valid
    bool IsValid() const;
    /// setup renderpass resource
    ResourceState::Code SetupResource(renderPass& rp);
    /// destroy renderpass resource
    void DestroyResource(renderPass& rp);

protected:
    gfxPointers pointers;
    bool isValid = false;

private:
    /// check render-pass for valid configuration
    #if ORYOL_DEBUG
    static void validateRenderPass(const renderPass& rp);
    #endif
};

} // namespace _priv
} // namespace Oryol
