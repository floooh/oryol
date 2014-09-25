#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBase
    @ingroup _priv
    @brief private: base class for platform-specific texture implementation
*/
#include "Resource/resourceBase.h"
#include "Gfx/Attrs/TextureAttrs.h"
#include "Gfx/Setup/TextureSetup.h"
#include "Gfx/Core/Enums.h"
#include "IO/IOProtocol.h"

namespace Oryol {
namespace _priv {

class textureBase : public resourceBase<TextureSetup> {
public:
    /// constructor
    textureBase();
    
    /// get (optional) IORequest
    const Ptr<IOProtocol::Request>& GetIORequest() const;
    /// get texture attrs object
    const TextureAttrs& GetTextureAttrs() const;

    /// clear the object
    void clear();
    /// set IO request for asynchronous loading
    void setIORequest(const Ptr<IOProtocol::Request>& ioRequest);
    /// set texture attributes object
    void setTextureAttrs(const TextureAttrs& attrs);
    
protected:
    Ptr<IOProtocol::Request> ioRequest;
    TextureAttrs textureAttrs;
};

//------------------------------------------------------------------------------
inline const Ptr<IOProtocol::Request>&
textureBase::GetIORequest() const {
    return this->ioRequest;
}

//------------------------------------------------------------------------------
inline const TextureAttrs&
textureBase::GetTextureAttrs() const {
    return this->textureAttrs;
}

} // namespace _priv
} // namespace Oryol

    