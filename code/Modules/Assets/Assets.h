#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Assets Assets
    @brief asynchronous asset loading and sharing

    @class Oryol::Assets
    @ingroup Assets
    @brief Assets module facade
*/
#include <functional>
#include "Core/Types.h"
#include "IO/Stream/Stream.h"
#include "Assets/AssetId.h"
#include "Assets/AssetsSetup.h"
#include "Assets/Core/assetRegistry.h"
#include "Assets/Core/AssetLoader.h"

namespace Oryol {
class Assets {
public:
    /// asset discard function
    typedef std::function<void(const Id&)> DiscardFunc;

    /// setup the Asset module
    static void Setup(const AssetsSetup& setup = AssetsSetup());
    /// discard the Asset module
    static void Discard();
    /// return true if Asset module is setup
    static bool IsValid();
    /// return the AssetSetup object handed to Setup method
    static const class AssetsSetup& AssetsSetup();
    
    /// attach an asset loader
    static void AttachLoader(const Ptr<AssetLoader>& loader);
    /// detach an asset loader
    static void DetachLoader(const Ptr<AssetLoader>& loader);
    
    /// async load shared asset (or return existing shared asset)
    template<class SETUP> static AssetId Load(const SETUP& setup);
    /// lookup a shared asset by locator (bumps use-count)
    static AssetId Lookup(const Locator& loc);
    /// register an existing resource as shared asset
    static AssetId Register(const Locator& loc, Id resId, DiscardFunc discardFunc);

private:
    friend class AssetId;
    
    /// increment use count of asset
    static void useAsset(const Id& assetId);
    /// decrement use count of asset
    static void releaseAsset(const Id& assetId);

    struct _state {
        class AssetsSetup setup;
        _priv::assetRegistry registry;
    };
    static _state* state;
};

//------------------------------------------------------------------------------
inline void
Assets::releaseAsset(const Id& resId) {
    if (IsValid()) {
        state->registry.Release(resId);
    }
}

//------------------------------------------------------------------------------
inline void
Assets::useAsset(const Id& resId) {
    if (IsValid()) {
        state->registry.Use(resId);
    }
}

} // namespace Asset
