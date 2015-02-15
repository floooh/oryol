#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::assetCreator
    @ingroup Assets
    @brief wrapper class for directly creating assets
    
    This class contains entirely of specialized template methods
    to forward a resource creation request to a specific Module
    and return the resulting resource id as AssetId. Each set of
    specialization methods should live in its own source code 
    (compilation unit) to make sure that unused Modules are not
    linked in.
*/
#include "IO/Stream/Stream.h"
#include "Assets/Core/assetRegistry.h"

namespace Oryol {
namespace _priv {

class assetCreator {
public:
    /// setup the asset creator
    void Setup(assetRegistry* registry);
    /// discard the asset creator
    void Discard();

    /// directly create a shared, use-counted asset
    template<class SETUP> Id Create(const SETUP& setup, assetRegistry* registry);
    /// directly create a shared, use-count asset with data
    template<class SETUP> Id Create(const SETUP& setup, const Ptr<Stream>& data, assetRegistry* registry);
    /// allocate a resource (for async resource loading)
    template<class SETUP> Id Alloc(const SETUP& setup, Id placeholder, assetRegistry* registry);
    /// setup a previously allocated resource from data (for async resource loading)
    template<class SETUP> void Init(const Id& id, const SETUP& setup, const Ptr<Stream>& data);

    /// one-time register a push-label function
    void regFuncsOnce(void(*push)(uint8), uint8(*pop)(), void(*discard)(uint8));
};

} // namespace _priv
} // namespace Oryol