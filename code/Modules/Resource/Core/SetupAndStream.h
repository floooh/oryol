#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::SetupAndStream
    @ingroup Gfx
    @brief a typle which holds a setup and a stream object
    
    This is used to transfer both a resource setup object and 
    a stream object to resource creation functions.
*/
#include "IO/Stream/Stream.h"

namespace Oryol {

template<class SETUP> class SetupAndStream {
public:
    /// default constructor
    SetupAndStream() { };
    /// construct from Setup and Stream object
    SetupAndStream(const SETUP& setup, const Ptr<class Stream>& stream) :
        Setup(setup),
        Stream(stream) {
        // empty
    };
    
    /// embedded setup object
    SETUP Setup;
    /// embedded stream object
    Ptr<class Stream> Stream;
};

} // namespace Oryol