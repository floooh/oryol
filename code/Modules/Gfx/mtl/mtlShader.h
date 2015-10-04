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

    /// set library object
    void setLibrary(ORYOL_OBJC_TYPED_ID(MTLLibrary) lib);
    /// get library object
    ORYOL_OBJC_TYPED_ID(MTLLibrary) getLibrary() const;

    /// add vs/fs function pair with selection mask
    int32 addShaders(uint32 mask, ORYOL_OBJC_TYPED_ID(MTLFunction) vsFunc, ORYOL_OBJC_TYPED_ID(MTLFunction) fsFunc);

    /// get number of programs
    int32 getNumPrograms() const;
    /// get prog index by mask (return InvalidIndex if not found)
    int32 getProgIndexByMask(uint32 mask) const;
    /// get vertex shader function at program index
    ORYOL_OBJC_TYPED_ID(MTLFunction) getVertexShader(int32 progIndex) const;
    /// get fragment shader function at program index
    ORYOL_OBJC_TYPED_ID(MTLFunction) getFragmentShader(int32 origIndex) const;

    class programEntry {
    public:
        uint32 mask;
        ORYOL_OBJC_TYPED_ID(MTLFunction) mtlVertexShader;
        ORYOL_OBJC_TYPED_ID(MTLFunction) mtlFragmentShader;
    };
    int32 numPrograms;
    StaticArray<programEntry, GfxConfig::MaxNumBundlePrograms> programEntries;
    ORYOL_OBJC_TYPED_ID(MTLLibrary) mtlLibrary;
};

} // namespace _priv
} // namespace Oryol
