#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::SetupAndData
    @ingroup Resource
    @brief holds a setup and a data buffer object
    
    This is used to transfer both a resource setup object and 
    a stream object to resource creation functions.
*/
#include "Core/Containers/Buffer.h"

namespace Oryol {

template<class SETUP> class SetupAndData {
public:
    /// default constructor
    SetupAndData() { };
    /// construct from Setup and Stream object
    SetupAndData(const SETUP& setup, Buffer&& data) :
        Setup(setup),
        Data(std::move(data)) {
        // empty
    };
    
    /// embedded setup object
    SETUP Setup;
    /// embedded data buffer
    Buffer Data;
};

} // namespace Oryol