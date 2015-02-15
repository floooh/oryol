#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::AssetLoader
    @ingroup Assets
    @brief base class for asset loaders
*/
#include "Core/RefCounted.h"
#include "Resource/Id.h"
#include "IO/Stream/Stream.h"

namespace Oryol {
class AssetLoader : public RefCounted {
    OryolClassDecl(AssetLoader);
public:
    /// called when attached to Assets module
    virtual void Attached();
    /// called when detached from Assets module
    virtual void Detached();
    /// try loading/creating the resource, return false if not possible
    virtual bool TryLoad(const Id& id, const Ptr<Stream>& data);
};
} // namespace Oryol
