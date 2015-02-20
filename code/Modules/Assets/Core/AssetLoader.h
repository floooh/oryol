#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::AssetLoader
    @ingroup Assets
    @brief base class for asset loaders
    
    An AssetLoader takes a resource setup object and a data stream (with
    the data stream usually containing data in a specific file format,
    like dds, obj, etc...), and returns another resource setup
    object and data stream where the data stream is in a canonical
    format.
*/
#include "Core/RefCounted.h"
#include "Resource/Id.h"
#include "IO/Stream/Stream.h"

namespace Oryol {
class AssetLoader : public RefCounted {
    OryolClassDecl(AssetLoader);
public:
    /// asset loader result

    /// called when attached to Assets module
    virtual void Attached();
    /// called when detached from Assets module
    virtual void Detached();
    /// try loading/creating the resource, return false if not possible
    virtual bool TryLoad(const Id& id, const Ptr<Stream>& data);
};
} // namespace Oryol
