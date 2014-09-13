#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBase
    @ingroup _priv
    @brief private: base class for platform-specific texture implementation
*/
#include "Resource/resourceBase.h"
#include "Render/Attrs/TextureAttrs.h"
#include "Render/Setup/TextureSetup.h"
#include "Render/Core/Enums.h"
#include "IO/IOProtocol.h"

namespace Oryol {
namespace _priv {

class textureBase : public Resource::resourceBase<TextureSetup> {
public:
    /// constructor
    textureBase();
    
    /// get (optional) IORequest
    const Ptr<IOProtocol::Get>& GetIORequest() const;
    /// get texture attrs object
    const TextureAttrs& GetTextureAttrs() const;

    /// clear the object
    void clear();
    /// set IO request for asynchronous loading
    void setIORequest(const Ptr<IOProtocol::Get>& ioRequest);
    /// set texture attributes object
    void setTextureAttrs(const TextureAttrs& attrs);
    
protected:
    Ptr<IOProtocol::Get> ioRequest;
    TextureAttrs textureAttrs;
};

//------------------------------------------------------------------------------
inline const Ptr<IOProtocol::Get>&
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

    