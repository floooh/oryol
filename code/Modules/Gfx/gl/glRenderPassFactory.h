#pragma once
//------------------------------------------------------------------------------
/** 
    @class Oryol::_priv::glRenderPassFactory
    @ingroup _priv
    @brief GL implementation of renderPassFactory
*/
#include "Gfx/Resource/renderPassFactoryBase.h"

namespace Oryol {
namespace _priv {

class glRenderPassFactory : public renderPassFactoryBase {
public:
    /// setup new renderPass object
    ResourceState::Code SetupResource(renderPass& rp);
    /// destroy renderPass object
    void DestroyResource(renderPass& rp);
};

} // namespace _priv
} // namespace Oryol

