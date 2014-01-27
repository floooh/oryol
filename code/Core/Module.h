#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Module

    Setup and discard the Core module.
*/
#include "Core/Types.h"

namespace Oryol {
namespace Core {

class Module {
public:
    /// setup the core system
    static void Setup();
    /// discard the core system
    static void Discard();
    /// must be called at start of a thread
    static void EnterThread();
    /// must be called right before a thread is left
    static void LeaveThread();
};

} // namespace Core
} // namespace Oryol