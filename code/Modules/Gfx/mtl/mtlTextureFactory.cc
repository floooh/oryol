//------------------------------------------------------------------------------
//  mtlTextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlTextureFactory.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlTextureFactory::mtlTextureFactory() :
renderer(nullptr),
displayManager(nullptr),
texPool(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlTextureFactory::~mtlTextureFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::Setup(class renderer* rendr, displayMgr* displayMgr, texturePool* texPool_) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(nullptr != rendr);
    o_assert_dbg(nullptr != displayMgr);
    o_assert_dbg(nullptr != texPool_);

    this->isValid = true;
    this->renderer = rendr;
    this->displayManager = displayMgr;
    this->texPool = texPool_;
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::Discard() {
    o_assert_dbg(this->isValid);

    this->isValid = false;
    this->renderer = nullptr;
    this->displayManager = nullptr;
    this->texPool = nullptr;
}

//------------------------------------------------------------------------------
bool
mtlTextureFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureFactory::SetupResource(texture& tex) {
    o_assert_dbg(this->isValid);

    o_error("FIXME: mtlTextureFactory::SetupResource()\n");
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureFactory::SetupResource(texture& tex, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    
    o_error("FIXME: mtlTextureFactory::SetupResource()\n");
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);
    
    o_error("FIXME: mtlTextureFactory::DestroyResource()\n");
}

} // namespace _priv
} // namespace Oryol