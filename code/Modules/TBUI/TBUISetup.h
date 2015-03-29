#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TBUISetup
    @brief setup parameters for the TBUI module
*/
#include "Core/Types.h"

namespace Oryol {

class TBUISetup {
public:
    /// resource pool size
    int32 ResourcePoolSize = 256;
    /// initial resource label stack capacity
    int32 ResourceLabelStackCapacity = 256;
    /// initial resource registry capacity
    int32 ResourceRegistryCapacity = 256;
};

} // namespace Oryol