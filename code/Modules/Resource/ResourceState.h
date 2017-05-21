#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ResourceState
    @ingroup Resource
    @brief current state of a resource object
    
    These are the states a resource object goes through during its lifetime:
    
    * Initial:  resource object has just been created
    * Setup:    resource object has a valid Setup object, but is not loaded
    * Pending:  asynchronous loading is underway
    * Valid:    resource is valid and can be used
    * Failed:   resource creation has failed
    
    Resources can be unloaded, which changes the state from Valid back
    to Setup, and then be loaded again.
*/
namespace Oryol {
    
class ResourceState {
public:
    /// state codes
    enum Code {
        Initial,    ///< resource has just been created
        Setup,      ///< the resource has a setup object, but is not loaded
        Pending,    ///< resource is pending (asynchronous loading)
        Valid,      ///< resource has become valid
        Failed,     ///< resource creation has failed
    
        NumStates,
        InvalidState,
    };

    /// convert resource State to string
    static const char* ToString(Code c) {
        switch (c) {
            case Initial:   return "Initial";
            case Setup:     return "Setup";
            case Pending:   return "Pending";
            case Valid:     return "Valid";
            case Failed:    return "Failed";
            default:        return "InvalidState";
        }
    }
};
    
} // namespace Oryol

