#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlShader
    @ingroup _priv
    @brief Metal implementation of class shader
*/
#include "Gfx/Resource/shaderBase.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class mtlShader : public shaderBase {
public:
    /// constructor
    mtlShader();
    /// destructor
    ~mtlShader();

    /// clear the object
    void Clear();

    ORYOL_OBJC_TYPED_ID(MTLLibrary) mtlLibrary;
    ORYOL_OBJC_TYPED_ID(MTLFunction) mtlVertexShader;
    ORYOL_OBJC_TYPED_ID(MTLFunction) mtlFragmentShader;
};

} // namespace _priv
} // namespace Oryol
