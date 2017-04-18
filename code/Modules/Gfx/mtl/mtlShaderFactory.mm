//------------------------------------------------------------------------------
//  mtlShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlShaderFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/resource.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlShaderFactory::~mtlShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlShaderFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
mtlShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
mtlShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nil == shd.mtlVertexShaderLibrary);
    o_assert_dbg(nil == shd.mtlFragmentShaderLibrary);

    const ShaderLang::Code slang = ShaderLang::Metal;
    const ShaderSetup& setup = shd.Setup;
    const void* vsLibraryByteCode = nullptr;
    uint32_t vsLibraryByteCodeSize = 0;
    setup.VertexShaderByteCode(slang, vsLibraryByteCode, vsLibraryByteCodeSize);
    o_assert_dbg(vsLibraryByteCode && (vsLibraryByteCodeSize > 0));
    const void* fsLibraryByteCode = nullptr;
    uint32_t fsLibraryByteCodeSize = 0;
    setup.FragmentShaderByteCode(slang, fsLibraryByteCode, fsLibraryByteCodeSize);
    o_assert_dbg(fsLibraryByteCode && (fsLibraryByteCodeSize > 0));

    // first create the shader library (one library per bundle)
    NSError* err = 0;
    dispatch_data_t vsLibData = dispatch_data_create(vsLibraryByteCode, vsLibraryByteCodeSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    shd.mtlVertexShaderLibrary = [this->pointers.renderer->mtlDevice newLibraryWithData:vsLibData error:&err];
    o_assert(nil == err);
    dispatch_data_t fsLibData = dispatch_data_create(fsLibraryByteCode, fsLibraryByteCodeSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    shd.mtlFragmentShaderLibrary = [this->pointers.renderer->mtlDevice newLibraryWithData:fsLibData error:&err];
    o_assert(nil == err);

    // create vertex and fragment shader function objects
    const StringAtom& vsName = setup.VertexShaderFunc(slang);
    shd.mtlVertexShader = [shd.mtlVertexShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:vsName.AsCStr()]];
    const StringAtom& fsName = setup.FragmentShaderFunc(slang);
    shd.mtlFragmentShader = [shd.mtlFragmentShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:fsName.AsCStr()]];

    // get the vertex shader attribute locations
    shd.vsAttrIndices.Fill(InvalidIndex);
    for (MTLVertexAttribute* mtlAttr in shd.mtlVertexShader.vertexAttributes) {
        if (mtlAttr.active) {
            VertexAttr::Code attr = VertexAttr::FromString(mtlAttr.name.UTF8String);
            if (VertexAttr::InvalidVertexAttr != attr) {
                shd.vsAttrIndices[attr] = int(mtlAttr.attributeIndex);
            }
        }
    }

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);
    if (nil != shd.mtlVertexShader) {
        this->pointers.renderer->releaseDeferred(shd.mtlVertexShader);
    }
    if (nil != shd.mtlFragmentShader) {
        this->pointers.renderer->releaseDeferred(shd.mtlFragmentShader);
    }
    if (nil != shd.mtlVertexShaderLibrary) {
        this->pointers.renderer->releaseDeferred(shd.mtlVertexShaderLibrary);
    }
    if (nil != shd.mtlFragmentShaderLibrary) {
        this->pointers.renderer->releaseDeferred(shd.mtlFragmentShaderLibrary);
    }
    shd.Clear();
}

} // namespace _priv
} // namespace Oryol
