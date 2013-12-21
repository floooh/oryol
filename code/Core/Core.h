#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Core.h

    Define core functions for Oryol.
*/
#include "Core/Types.h"

namespace Oryol {
namespace Core {

/// setup the core system
void Setup();
/// discard the core system
void Discard();
/// must be called at start of a thread
void EnterThread();
/// must be called right before a thread is left
void LeaveThread();

} // namespace Core
} // namespace Oryol