#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlProgramBundle
    @ingroup _priv
    @brief Metal implementation of class programBundle
*/
#include "Gfx/Resource/programBundleBase.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class mtlProgramBundle : public programBundleBase {
public:
    /// constructor
    mtlProgramBundle();
    /// destructor
    ~mtlProgramBundle();

    /// clear the object
    void Clear();

    /// set library object
    void setLibrary(ORYOL_OBJC_TYPED_ID(MTLLibrary) lib);
    /// get libraru object
    ORYOL_OBJC_TYPED_ID(MTLLibrary) getLibrary() const;

    /// add vs/fs function pair with selection mask
    int32 addShaders(uint32 mask, ORYOL_OBJC_TYPED_ID(MTLFunction) vsFunc, ORYOL_OBJC_TYPED_ID(MTLFunction) fsFunc);
    /// select an vs/fs pair
    bool select(uint32 mask);
    /// get the current selection mask
    uint32 getSelectionMask() const;
    /// get the current selection index
    int32 getSelectionIndex() const;
    /// get the currently selected vertex shader function
    ORYOL_OBJC_TYPED_ID(MTLFunction) getSelectedVertexShader() const;
    /// get the currently selected fragment shader function
    ORYOL_OBJC_TYPED_ID(MTLFunction) getSelectedFragmentShader() const;

    /// get number of programs
    int32 getNumPrograms() const;
    /// get vertex shader function at program index
    ORYOL_OBJC_TYPED_ID(MTLFunction) getVertexShaderAt(int32 index) const;
    /// get fragment shader function at program index
    ORYOL_OBJC_TYPED_ID(MTLFunction) getFragmentShaderAt(int32 index) const;

    // FIXME: uniform blocks

private:
    class programEntry {
    public:
        uint32 mask;
        ORYOL_OBJC_TYPED_ID(MTLFunction) vertexShader;
        ORYOL_OBJC_TYPED_ID(MTLFunction) fragmentShader;
    };
    uint32 selMask;
    int32 selIndex;
    int32 numPrograms;
    StaticArray<programEntry, ProgramBundleSetup::MaxNumPrograms> programEntries;
    ORYOL_OBJC_TYPED_ID(MTLLibrary) library;
};

} // namespace _priv
} // namespace Oryol