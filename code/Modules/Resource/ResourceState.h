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
#include "Core/Types.h"

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
    static const char* ToString(Code c);
    /// convert string to resource State
    static Code FromString(const char* str);
};
    
} // namespace Oryol

