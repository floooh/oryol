#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::MeshLoader
    @ingroup Assets
    @brief standard mesh loader for loading .omsh files
    
    NOTE: .omsh files are created by the oryol-exporter tool
    in the project https://github.com/floooh/oryol-tools
*/
#include "Gfx/Resource/MeshLoaderBase.h"
#include "IO/IOProtocol.h"

namespace Oryol {

class MeshLoader : public MeshLoaderBase {
    OryolClassDecl(MeshLoader);
public:
    /// constructor without success-callback
    MeshLoader(const MeshSetup& setup, int32 ioLane);
    /// constructor with success callback
    MeshLoader(const MeshSetup& setup, int32 ioLane, LoadedFunc onLoaded);
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
    Ptr<IOProtocol::Read> ioRequest;
};

} // namespace Oryol
