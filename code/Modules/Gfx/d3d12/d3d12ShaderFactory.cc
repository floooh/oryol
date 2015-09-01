//------------------------------------------------------------------------------
//  d3d12ShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "d3d12ShaderFactory.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/shader.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12ShaderFactory::d3d12ShaderFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d12ShaderFactory::~d3d12ShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d12ShaderFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
d3d12ShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
d3d12ShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12ShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->isValid);

    this->pointers.renderer->invalidateShaderState();
    const ShaderLang::Code slang = ShaderLang::HLSL5;
    const ShaderSetup& setup = shd.Setup;

    // set vertex- and pixel-shader bytecode pointers
    d3d12Shader::shaderBlob vsByteCode;
    d3d12Shader::shaderBlob psByteCode;
    const int32 numProgs = setup.NumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {
        setup.VertexShaderByteCode(progIndex, slang, vsByteCode.ptr, vsByteCode.size);
        setup.FragmentShaderByteCode(progIndex, slang, psByteCode.ptr, psByteCode.size);
        shd.addShaders(setup.Mask(progIndex), vsByteCode, psByteCode);
    }

    // set uniform block binding info
    for (int i = 0; i < setup.NumUniformBlocks(); i++) {
        const ShaderType::Code bindShaderStage = setup.UniformBlockShaderStage(i);
        const int32 bindSlotIndex = setup.UniformBlockSlot(i);
        shd.addUniformBlockEntry(bindShaderStage, bindSlotIndex);
    }
    
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d12ShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);
    this->pointers.renderer->invalidateShaderState();
    shd.Clear();
}

} // namespace _priv
} // namespace Oryol

