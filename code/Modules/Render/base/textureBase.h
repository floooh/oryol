#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::textureBase
    @brief private: base class for platform-specific texture implementation
*/
#include "Resource/resourceBase.h"
#include "Render/Attrs/TextureAttrs.h"
#include "Render/Setup/TextureSetup.h"
#include "Render/Core/Enums.h"
#include "IO/IOProtocol.h"

namespace Oryol {
namespace Render {

class textureBase : public Resource::resourceBase<TextureSetup> {
public:
    /// constructor
    textureBase();
    
    /// get (optional) IORequest
    const Core::Ptr<IO::IOProtocol::Get>& GetIORequest() const;
    /// get texture attrs object
    const TextureAttrs& GetTextureAttrs() const;

    /// clear the object
    void clear();
    /// set IO request for asynchronous loading
    void setIORequest(const Core::Ptr<IO::IOProtocol::Get>& ioRequest);
    /// set texture attributes object
    void setTextureAttrs(const TextureAttrs& attrs);
    
protected:
    Core::Ptr<IO::IOProtocol::Get> ioRequest;
    TextureAttrs textureAttrs;
};

//------------------------------------------------------------------------------
inline const Core::Ptr<IO::IOProtocol::Get>&
textureBase::GetIORequest() const {
    return this->ioRequest;
}

//------------------------------------------------------------------------------
inline const TextureAttrs&
textureBase::GetTextureAttrs() const {
    return this->textureAttrs;
}

} // namespace Render
} // namespace Oryol

    