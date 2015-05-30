#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11DrawState
    @ingroup _priv
    @brief D3D11 implementation of drawState
*/
#include "Gfx/Resource/drawStateBase.h"
#include "Gfx/d3d11/d3d11_decl.h"
#include "Gfx/d3d11/d3d11ProgramBundle.h"

namespace Oryol {
namespace _priv {

class d3d11DrawState : public drawStateBase {
public:
    /// constructor
    d3d11DrawState();
    /// destructor
    ~d3d11DrawState();

    /// clear the object (called from drawStateFactory::DestroyResource())
    void Clear();

    /// pointer to input layout objects (same number and order as programBundle variations)
    ID3D11InputLayout* d3d11InputLayouts[d3d11ProgramBundle::MaxNumPrograms];
};

} // namespace _priv
} // namespace Oryol