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
    /// move construct
    SetupAndData(SetupAndData<SETUP>&& rhs) {
        this->Setup = std::move(rhs.Setup);
        this->Data = std::move(rhs.Data);
    };
    /// move assignment
    void operator=(SetupAndData<SETUP>&& rhs) {
        this->Setup = std::move(rhs.Setup);
        this->Data = std::move(rhs.Data);        
    };

    /// disable copy constructor
    SetupAndData(const SetupAndData<SETUP>& rhs) = delete;
    /// disable copy assignment
    void operator=(const SetupAndData<SETUP>& rhs) = delete;
    
    /// embedded setup object
    SETUP Setup;
    /// embedded data buffer
    Buffer Data;
};

} // namespace Oryol