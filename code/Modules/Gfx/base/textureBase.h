#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBase
    @ingroup _priv
    @brief private: base class for platform-specific texture implementation
*/
#include "Resource/Core/resourceBase.h"
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
    
    /// texture attributes
    TextureAttrs textureAttrs;
    
    /// get (optional) IORequest
    const Ptr<IOProtocol::Request>& GetIORequest() const;

    /// clear the object
    void clear();
    /// set IO request for asynchronous loading
    void setIORequest(const Ptr<IOProtocol::Request>& ioRequest);
    
protected:
    Ptr<IOProtocol::Request> ioRequest;
};

//------------------------------------------------------------------------------
inline const Ptr<IOProtocol::Request>&
textureBase::GetIORequest() const {
    return this->ioRequest;
}

} // namespace _priv
} // namespace Oryol

    