//------------------------------------------------------------------------------
//  MeshLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshLoader.h"
#include "Assets/Gfx/OmshParser.h"
#include "Gfx/Gfx.h"
#include "Gfx/private/gfxResourceContainer.h"
#include "IO/IO.h"

namespace Oryol {

//------------------------------------------------------------------------------
MeshLoader::MeshLoader(const MeshSetup& setup_) :
MeshLoaderBase(setup_) {
    // empty
}

//------------------------------------------------------------------------------
MeshLoader::MeshLoader(const MeshSetup& setup_, LoadedFunc loadedFunc_) :
MeshLoaderBase(setup_, loadedFunc_) {
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
        this->ioRequest->Cancelled = true;
        this->ioRequest = nullptr;
    }
}

//------------------------------------------------------------------------------
Id
MeshLoader::Start() {
    this->resId = Gfx::resource()->prepareAsync(this->setup);
    this->ioRequest = IO::LoadFile(setup.Locator.Location());
    return this->resId;
}

//------------------------------------------------------------------------------
ResourceState::Code
MeshLoader::Continue() {
    o_assert_dbg(this->resId.IsValid());
    o_assert_dbg(this->ioRequest.isValid());
    
    ResourceState::Code result = ResourceState::Pending;
    
    if (this->ioRequest->Handled) {
        if (IOStatus::OK == this->ioRequest->Status) {
            // async loading has finished, use OmshParser to
            // create a MeshSetup object from the loaded data
            const void* data = this->ioRequest->Data.Data();
            const int numBytes = this->ioRequest->Data.Size();

            MeshSetup meshSetup = MeshSetup::FromData(this->setup);
            if (OmshParser::Parse(data, numBytes, meshSetup)) {

                // call the Loaded callback if defined, this
                // gives the app a chance to look at the
                // setup object, and possibly modify it
                if (this->onLoaded) {
                    this->onLoaded(meshSetup);
                }

                // NOTE: the prepared resource might have already been
                // destroyed at this point, if this happens, initAsync will
                // silently fail and return ResourceState::InvalidState
                // (the same for failedAsync)
                result = Gfx::resource()->initAsync(this->resId, meshSetup, data, numBytes);
            }
            else {
                result = Gfx::resource()->failedAsync(this->resId);
            }
        }
        else {
            // IO had failed
            result = Gfx::resource()->failedAsync(this->resId);
        }
        this->ioRequest = nullptr;
    }
    return result;
}

} // namespace Oryol
