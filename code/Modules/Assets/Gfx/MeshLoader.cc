//------------------------------------------------------------------------------
//  MeshLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshLoader.h"
#include "Assets/Gfx/OmshParser.h"
#include "Gfx/Gfx.h"
#include "IO/IO.h"

namespace Oryol {

OryolClassImpl(MeshLoader);

//------------------------------------------------------------------------------
MeshLoader::MeshLoader(const MeshSetup& setup_, int32 ioLane_) :
MeshLoaderBase(setup_, ioLane_) {
    // empty
}

//------------------------------------------------------------------------------
MeshLoader::~MeshLoader() {
    o_assert_dbg(!this->ioRequest);
}

//------------------------------------------------------------------------------
void
MeshLoader::Cancel() {
    if (this->ioRequest) {
        this->ioRequest->SetCancelled();
        this->ioRequest = nullptr;
    }
}

//------------------------------------------------------------------------------
Id
MeshLoader::Start() {
    
    // prepare the Gfx resource
    this->resId = Gfx::resource().prepareAsync(this->setup);
    
    // fire IO request to start loading the texture data
    this->ioRequest = IOProtocol::Request::Create();
    this->ioRequest->SetURL(setup.Locator.Location());
    this->ioRequest->SetLane(this->ioLane);
    IO::Put(this->ioRequest);
    
    return this->resId;
}

//------------------------------------------------------------------------------
ResourceState::Code
MeshLoader::Continue() {
    o_assert_dbg(this->resId.IsValid());
    o_assert_dbg(this->ioRequest.isValid());
    
    ResourceState::Code result = ResourceState::Pending;
    
    if (this->ioRequest->Handled()) {
        if (this->ioRequest->GetStatus() == IOStatus::OK) {
            // async loading has finished, use OmshParser to
            // create a MeshSetup object from the loaded data
            const Ptr<Stream>& stream = this->ioRequest->GetStream();
            stream->Open(OpenMode::ReadOnly);
            const void* data = stream->MapRead(nullptr);
            const int32 numBytes = stream->Size();

            MeshSetup meshSetup = MeshSetup::FromData(this->setup);
            if (OmshParser::Parse(data, numBytes, meshSetup)) {
                stream->Close();
                // NOTE: the prepared texture resource might have already been
                // destroyed at this point, if this happens, initAsync will
                // silently fail and return ResourceState::InvalidState
                // (the same for failedAsync)
                result = Gfx::resource().initAsync(this->resId, meshSetup, data, numBytes);
            }
            else {
                stream->Close();
                result = Gfx::resource().failedAsync(this->resId);
            }
        }
        else {
            // IO had failed
            result = Gfx::resource().failedAsync(this->resId);
        }
        this->ioRequest = nullptr;
    }
    return result;
}

} // namespace Oryol
