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

    /// add a uniform block entry
    void addUniformBlock(ShaderType::Code bindShaderStage, int32 bindSlotIndex);
    /// get number of uniform blocks
    int32 getNumUniformBlocks() const;
    /// get uniform block shader stage
    ShaderType::Code getUniformBlockShaderStage(int32 ubIndex) const;
    /// get uniform block bind slot
    int32 getUniformBlockBindSlotIndex(int32 ubIndex) const;

    class programEntry {
    public:
        uint32 mask;
        ORYOL_OBJC_TYPED_ID(MTLFunction) mtlVertexShader;
        ORYOL_OBJC_TYPED_ID(MTLFunction) mtlFragmentShader;
    };
    struct ubEntry {
        ShaderType::Code bindShaderStage = ShaderType::InvalidShaderType;
        int32 bindSlotIndex;
    };
    uint32 selMask;
    int32 selIndex;
    int32 numPrograms;
    StaticArray<programEntry, GfxConfig::MaxNumBundlePrograms> programEntries;
    ORYOL_OBJC_TYPED_ID(MTLLibrary) mtlLibrary;
    int32 numUniformBlockEntries;
    StaticArray<ubEntry, GfxConfig::MaxNumUniformBlocks> uniformBlockEntries;
};

} // namespace _priv
} // namespace Oryol