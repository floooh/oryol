#pragma once
//------------------------------------------------------------------------------
/**
    @class Assets::assetLoaderRegistry
    @ingroup _priv
    @brief manages attached asset loaders
*/
#include "Core/RunLoop.h"
#include "Assets/Core/AssetLoader.h"
#include "IO/IOProtocol.h"

namespace Oryol {
namespace _priv {

class assetLoaderRegistry {
public:
    /// constructor
    assetLoaderRegistry();
    /// destructor
    ~assetLoaderRegistry();

    /// setup the asset loader registry
    void Setup();
    /// discard the asset loader registry
    void Discard();
    /// return true if asset loader registry has been setup
    bool IsValid() const;
    
    /// attach an asset loader
    void AttachLoader(const Ptr<AssetLoader>& loader);
    /// detach an asset loader (pass by copy is NOT a bug!)
    void DetachLoader(Ptr<AssetLoader> loader);
    /// async load shared asset
    template<class SETUP> static Id Load(const SETUP& setup);
    
private:
    /// per-frame update
    void update();

    bool isValid;
    Array<Ptr<AssetLoader>> loaders;
    struct request {
        Id id;
        Ptr<IOProtocol::Request> ioRequest;
    };
    Array<request> pendingRequests;
    RunLoop::Id runLoopId;
};

} // namespace _priv
} // namespace Oryol