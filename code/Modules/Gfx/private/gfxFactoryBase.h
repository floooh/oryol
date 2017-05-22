#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::gfxFactoryBase
    @ingroup _priv
    @brief base class for Gfx resource factories
*/
#include "Resource/ResourceState.h"
#include "Gfx/private/gfxPointers.h"
#include "Gfx/GfxTypes.h"

namespace Oryol {
namespace _priv {

class pipeline;
class renderPass;

class gfxFactoryBase {
public:
    /// destructor
    ~gfxFactoryBase();

    /// setup the factory
    void setup(const gfxPointers& ptrs);
    /// discard the factory
    void discard();
    /// optional garbage-collect released resources (not implemented on all platforms)
    void garbageCollect();
    /// initialize pipeline object
    ResourceState::Code initPipeline(pipeline& pip);
    /// destroy pipeline object
    void destroyPipeline(pipeline& pip);
    /// initialize renderPass object
    ResourceState::Code initRenderPass(renderPass& rp);
    /// destroy renderPass object
    void destroyRenderPass(renderPass& rp);

    gfxPointers pointers;
    bool isValid = false;
};

} // namespace _priv
} // namespace Oryol
