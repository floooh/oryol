#pragma once
//------------------------------------------------------------------------------
/**
    @file core/core.h

    Define core functions for Oryol.
*/
#include "core/types.h"

namespace oryol
{
namespace core {

/// setup the core system
void setup();
/// discard the core system
void discard();
/// must be called at start of a thread
void enter_thread();
/// must be called right before a thread is left
void leave_thread();

} // namespace core
} // namespace oryol