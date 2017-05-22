#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::MeshLoader
    @ingroup Assets
    @brief standard mesh loader for loading .omsh files
    
    NOTE: .omsh files are created by the oryol-exporter tool
    in the project https://github.com/floooh/oryol-tools
*/
#include "Gfx/MeshLoaderBase.h"
#include "IO/private/ioRequests.h"

namespace Oryol {

class MeshLoader : public MeshLoaderBase {
    OryolClassDecl(MeshLoader);
public:
    /// constructor without success-callback
    MeshLoader(const MeshSetup& setup);
    /// constructor with success callback
    MeshLoader(const MeshSetup& setup, LoadedFunc onLoaded);
    /// destructor
    ~MeshLoader();
    /// start loading, return a resource id
    virtual Id Start() override;
    /// continue loading, return resource state (Pending, Valid, Failed)
    virtual ResourceState::Code Continue() override;
    /// cancel the load process
    virtual void Cancel() override;
private:
    Id resId;
    Ptr<IORead> ioRequest;
};

} // namespace Oryol
