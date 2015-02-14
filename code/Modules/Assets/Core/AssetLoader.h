#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::AssetLoader
    @ingroup Assets
    @brief base class for asset loaders
*/
#include "Assets/AssetFormat.h"

namespace Oryol {
class AssetLoader {
public:
    /// called when attached to Assets module
    virtual void Attached() = 0;
    /// called when detached from Assets module
    virtual void Detached() = 0;
    /// try loading/creating the resource, return false if not possible
    virtual bool TryLoad(const Id& id, const Ptr<Stream>& data);
};
} // namespace Oryol
